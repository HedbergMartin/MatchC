#include "testing_utils.h"

#include <stdarg.h>
#include "parser.h"
#include "string.h"
#include "stdio.h"
#include "time.h"

match_entry* create_ref_match(char* pattern, vector* v) {
    match_entry* nm = calloc(1, sizeof(match_entry));
    nm->pattern = pattern;
    nm->subst_amount = vector_size(v);
    nm->substitutions = malloc(nm->subst_amount * sizeof(substitution)); //s_vector_copy(v, &(nm->subst_amount));
    
    substitution* s = NULL;
    for (int i = 0; i < nm->subst_amount; i++) {
        s = vector_at(v, i);
        nm->substitutions[i].from = s->from;
        nm->substitutions[i].to = s->to;
        nm->substitutions[i].len = s->len;
    }

    return nm;
}

void add_subst(vector* v, char* from, int len, ...) {
    va_list args;
    va_start(args, len);
 
    subjectFlatterm* prevft = NULL;
    subjectFlatterm* firstft = NULL;
    int i = 0;
    for (i = 0; i < len; i++) {
        char* to = va_arg(args, char*);
        
        subjectFlatterm* ft = calloc(1, sizeof(subjectFlatterm));
        ft->symbol = to;

        if (firstft == NULL) {
            firstft = ft;
        }
        if (prevft != NULL) {
            prevft->skip = ft;
        }
        prevft = ft;
    }

    substitution* s = malloc(sizeof(substitution));
    s->from = from;
    s->to = firstft;
    s->len = i;
    
    vector_push_back(v, s);
 
    va_end(args);
}

int compare_subst(substitution* su, substitution* suRef, int debug) {
    if (debug) {
        printf("From: %s(%s), Len: %d(%d), To: ", su->from, suRef->from, su->len, suRef->len);
    }

    if (su->len != suRef->len) {
        return 0;
    }
    
    if (strcmp(su->from, suRef->from) != 0) {
        return 0;
    }


    subjectFlatterm* ft = su->to;
    subjectFlatterm* ftRef = suRef->to;
    for (int j = 0; j < su->len; j++) {
        if (debug) {
            printf("%s(%s)", ft->symbol, ftRef->symbol);
        }

        if (strcmp(ft->symbol, ftRef->symbol) != 0) {
            return 0;
        }
        ft = ft->skip;
        ftRef = ftRef->skip;
    }

    return 1;
}

int valid_match(match_entry* match, vector* refmatches, int debug) {
    char str[4096] = "";
    for (int i = 0; i < vector_size(refmatches); i++) {
        str[0] = '\0';
        match_entry* refMatch = (match_entry*)vector_at(refmatches, i);

        if (debug) {
            //fprintf(str, "MatchID: %d(%d)\n", match->matchid, refMatch->matchid);
        }

        // if (match->matchid != refMatch->matchid) {
        //     continue;
        // }

        for (int i = 0; i < match->subst_amount; i++) {
            substitution* su = &(match->substitutions[i]);
            substitution* suRef = &(refMatch->substitutions[i]);

            if (!compare_subst(su, suRef, debug)) {
                continue;
            }
            
            if (debug) {
                //fprintf(str, "\n");
            }
        }
        
        printf("%s", str);
        return 1;
    }

    return 0;
}

int test_net(char* patterns[], char* subject, vector* refmatches, int debug) {
	d_net* net = net_init();

    int i = 0;
    while (patterns[i] != NULL) {
        flatterm* ft = parsePattern(patterns[i]);
        add_pattern(net, ft);
        i++;
    }

	// subjectFlatterm* ft_subject = parse_subject(subject, getSymbolHt(net));
	
	vector* matches = pattern_match(net, subject);

    if (vector_size(matches) != vector_size(refmatches)) {

        fprintf(stderr, "miss match on matches: %ld vs refmatches: %ld\n", vector_size(matches), vector_size(refmatches));
        vector_free(matches, free);
        return 1;
    }

    int res = 0;

	for (int i = 0; i < vector_size(matches); i++) {
		match_entry* match = (match_entry*)vector_at(matches, i);
        if (!valid_match(match, refmatches, debug)) {
            res = 1;
            break;
        }
	}
	// for (i = 0; i < vector_size(matches); i++) {
    //     char* m = (char*)vector_at(matches, i);
    //     // printf("%s == %s\n", shouldMatch[i], m);
    //     if (strcmp(shouldMatch[i], m) != 0) {
    //         res = 1;
    //         break;
    //     }
	// }

	vector_free(matches, free);
    return res;
}

void load_patterns(char* filename, d_net* net, double* parseTime, double* addTime ) {
    int MAXCHAR = 10000;
    FILE *fp;
    char str[MAXCHAR];

    fp = fopen(filename, "r");
    clock_t startParsePattern;
    clock_t endParsePattern;
    clock_t startAddPattern;
    clock_t endAddPattern;
    if (fp == NULL){
        fprintf(stderr, "Could not open file %s\n",filename);
    } else {
        int i = 0;
        while (fgets(str, MAXCHAR, fp) != NULL) {
            startParsePattern = clock();
            flatterm* ft = parsePattern(str);
            endParsePattern = clock();
            *parseTime += (double)(endParsePattern - startParsePattern) / CLOCKS_PER_SEC;
            if (ft == NULL) {
                fprintf(stderr, "Error on line: %d\n ", i);
            } else {
                startAddPattern = clock();
                add_pattern(net, ft);
                endAddPattern = clock();
                *addTime += (double)(endAddPattern - startAddPattern) / CLOCKS_PER_SEC;
            }
            //printf("%s", str);
            i += 1;
        }
        fclose(fp);
    }    
}

void load_subjects(char* filename, subjectFlatterm** subjects, int subjectCount, double* parseTime, d_net* net) {
    int MAXCHAR = 5000;
    FILE *fp;
    char str[MAXCHAR];

    clock_t startParseSubject;
    clock_t endParseSubject;
    fp = fopen(filename, "r");
    if (fp == NULL){
        fprintf(stderr, "Could not open file %s\n",filename);
    } else {
        int i = 0;
        while (fgets(str, MAXCHAR, fp) != NULL) {

            if (i >= subjectCount) {
                break;
            }
            startParseSubject = clock();
            subjectFlatterm* sf = parse_subject(str, getSymbolHt(net), net_nextId(net));
            endParseSubject = clock();
            *parseTime += (double)(endParseSubject - startParseSubject) / CLOCKS_PER_SEC;

            if (sf == NULL) {
                fprintf(stderr, "Error on line: %d\n ", i);
            } else {
                subjects[i] = sf;
                /*print_subjectFlatterm(sf);
                fprintf(stderr, "Successfully parsed line: %d\n", i);*/
            }
            i += 1;
        }
        fclose(fp);
    }    
}

void test_free(void* var) {
    match_entry* match = (match_entry*)var;
    free(match->substitutions);
    free(match);
}