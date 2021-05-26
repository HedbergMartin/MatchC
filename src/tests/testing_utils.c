#include "testing_utils.h"

#include <stdarg.h>
#include "parser.h"
#include "string.h"
#include "stdio.h"
#include "time.h"

net_match* create_match(int id, s_vector* v) {
    net_match* nm = calloc(1, sizeof(net_match));
    nm->matchid = id;
    nm->substitutions = s_vector_copy(v, &(nm->subst_amount));
}

void add_subst(s_vector* v, char* from, int len, ...) {
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
    
    s_vector_push_back(v, from, firstft, i);
 
    va_end(args);
}

int compare_subst(s_entry* su, s_entry* suRef, int debug) {
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

int valid_match(net_match* match, vector* refmatches, int debug) {
    char str[4096] = "";
    for (int i = 0; i < vector_size(refmatches); i++) {
        str[0] = '\0';
        net_match* refMatch = (net_match*)vector_at(refmatches, i);

        if (debug) {
            //fprintf(str, "MatchID: %d(%d)\n", match->matchid, refMatch->matchid);
        }

        // if (match->matchid != refMatch->matchid) {
        //     continue;
        // }

        for (int i = 0; i < match->subst_amount; i++) {
            s_entry* su = &(match->substitutions[i]);
            s_entry* suRef = &(refMatch->substitutions[i]);

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
	subjectFlatterm* ft_subject = parse_subject(subject);
	d_net* net = net_init();

    int i = 0;
    while (patterns[i] != NULL) {
        flatterm* ft = parsePattern(patterns[i]);
        add_pattern(net, ft);
        i++;
    }
	
	vector* matches = pattern_match(net, ft_subject);

    if (vector_size(matches) != vector_size(refmatches)) {

        fprintf(stderr, "miss match on matches: %ld vs refmatches: %ld\n", vector_size(matches), vector_size(refmatches));
        vector_free(matches, free);
        return 1;
    }

    int res = 0;

	for (int i = 0; i < vector_size(matches); i++) {
		net_match* match = (net_match*)vector_at(matches, i);
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
        fprintf(stderr, "Could not open file %s",filename);
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

void load_subjects(char* filename, subjectFlatterm** subjects, int subjectCount, double* parseTime) {
    int MAXCHAR = 5000;
    FILE *fp;
    char str[MAXCHAR];

    clock_t startParseSubject;
    clock_t endParseSubject;
    fp = fopen(filename, "r");
    if (fp == NULL){
        fprintf(stderr, "Could not open file %s",filename);
    } else {
        int i = 0;
        while (fgets(str, MAXCHAR, fp) != NULL) {

            if (i >= subjectCount) {
                break;
            }
            startParseSubject = clock();
            subjectFlatterm* sf = parse_subject(str);
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
    net_match* match = (net_match*)var;
    free(match->substitutions);
    free(match);
}