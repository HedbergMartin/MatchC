#include "ihct.h"
#include "d_net.h"
#include "parser.h"
#include "string.h"
#include "stdio.h"
#include "time.h"
#include <stdarg.h>

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
        return 1;
    }
    
    if (strcmp(su->from, suRef->from) != 0) {
        return 1;
    }


    subjectFlatterm* ft = su->to;
    subjectFlatterm* ftRef = suRef->to;
    for (int j = 0; j < su->len; j++) {
        if (debug) {
            printf("%s(%s)", ft->symbol, ftRef->symbol);
        }

        if (strcmp(ft->symbol, ftRef->symbol) != 0) {
            return 1;
        }
        ft = ft->skip;
        ftRef = ftRef->skip;
    }

    return 0;
}

int valid_match(net_match* match, vector* refmatches, int debug) {
    char str[4096] = "";
    for (int i = 0; i < vector_size(refmatches); i++) {
        str[0] = '\0';
        net_match* refMatch = (net_match*)vector_at(refmatches, i);

        if (debug) {
            //fprintf(str, "MatchID: %d(%d)\n", match->matchid, refMatch->matchid);
        }

        if (match->matchid != refMatch->matchid) {
            continue;
        }

        for (int i = 0; i < match->subst_amount; i++) {
            s_entry* su = &(match->substitutions[i]);
            s_entry* suRef = &(refMatch->substitutions[i]);

            if (compare_subst(su, suRef, debug) != 0) {
                continue;
            }
            
            if (debug) {
                //fprintf(str, "\n");
            }
        }
        
        printf("%s", str);
        return 0;
    }

    return 1;
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

IHCT_TEST(perf_test) {
    
    char* filenamePatterns = "../../patterns_100000.txt";
    char* filenameSubjects = "../../subjects_10000.txt";
    d_net* net = net_init();
    int subjectCount = 1000;

    double timeParsePattern = 0;
    double timeAddPattern = 0;
    double timeParseSubject = 0;
    double timeMatch = 0;

    clock_t matchStart;
    clock_t matchEnd;
 
    load_patterns(filenamePatterns, net, &timeParsePattern, &timeAddPattern);


    subjectFlatterm** subjects = malloc(sizeof(subjectFlatterm) * (subjectCount+1) );
    load_subjects(filenameSubjects, subjects, subjectCount, &timeParseSubject);

    for (int i = 0; i < subjectCount; i++) {
        //fprintf(stderr, "i: %d\n", i);
        //print_subjectFlatterm(subjects[i]);
        //printf("-----\nMatches:\n");
        matchStart = clock();
        vector* matches = pattern_match(net, subjects[i]);
        matchEnd = clock();
        timeMatch += (double)(matchEnd - matchStart) / CLOCKS_PER_SEC;

        // for (int i = 0; i < vector_size(matches); i++) {
        //     net_match* match = (net_match*)vector_at(matches, i);
        //     printf("MatchID: %d\n", match->matchid);
        //     for (int i = 0; i < match->subst_amount; i++) {
        //         s_entry* su = &(match->substitutions[i]);
        //         printf("From: %s, To: ", su->from);
        //         subjectFlatterm* ft = su->to;
        //         for (int j = 0; j < su->len; j++) {
        //             printf("%s, ", ft->symbol);
        //             ft = ft->skip;
        //         }
        //         printf("\n");
        //     }
        // }
        vector_free(matches, test_free);
    }

    fprintf(stderr, "timeParsePattern: %f\n", timeParsePattern);
    fprintf(stderr, "timeAddPattern: %f\n", timeAddPattern);
    fprintf(stderr, "timeParseSubject: %f\n", timeParseSubject);
    fprintf(stderr, "timeMatch: %f\n", timeMatch);

    IHCT_ASSERT(0 == 0);
    net_free(net);

    for (int i = 0; i < subjectCount; i++) {

        subjectFlatterm_free(subjects[i]);
    }
    free(subjects);
}

IHCT_TEST(variable_match) {
    char* patterns[] = {"f[x_]", NULL};
    s_vector* sus = s_vector_init();
    add_subst(sus, "x_", 1, "2");
    net_match* new_match = create_match(1, sus);

    vector* matches = vector_init();
    vector_push_back(matches, new_match);

    IHCT_ASSERT(test_net(patterns, "f[2]", matches, false) == 0);
}

IHCT_TEST(variable_repeating_match) {
    char* patterns[] = {"f[x_, x_]", NULL};
    s_vector* sus = s_vector_init();
    add_subst(sus, "x_", 1, "2");
    net_match* new_match = create_match(1, sus);

    vector* matches = vector_init();
    vector_push_back(matches, new_match);

    IHCT_ASSERT(test_net(patterns, "f[2, 2]", matches, false) == 0);
}

IHCT_TEST(variable_repeating_not_match1) {
    char* patterns[] = {"f[x_, x_]", NULL};
    s_vector* sus = s_vector_init();
    add_subst(sus, "x_", 1, "2");
    add_subst(sus, "x_", 1, "3");
    net_match* new_match = create_match(1, sus);

    vector* matches = vector_init();
    vector_push_back(matches, new_match);

    IHCT_ASSERT(test_net(patterns, "f[2, 3]", matches, false) == 1);
}

IHCT_TEST(variable_repeating_not_match2) {
    char* patterns[] = {"f[x_, x_]", NULL};
    s_vector* sus = s_vector_init();
    add_subst(sus, "x_", 1, "2");
    net_match* new_match = create_match(1, sus);

    vector* matches = vector_init();
    vector_push_back(matches, new_match);

    IHCT_ASSERT(test_net(patterns, "f[2, 3]", matches, false) == 1);
}

IHCT_TEST(variable_function_match) {
    char* patterns[] = {"f[x_]", NULL};

    s_vector* sus = s_vector_init();
    vector* matches = vector_init();

    add_subst(sus, "x_", 1, "g");
    vector_push_back(matches, create_match(2, sus));

    IHCT_ASSERT(test_net(patterns, "f[g[h[]]]", matches, false) == 0);
}

IHCT_TEST(sequence_match) {
    char* patterns[] = {"f[x___, y___]", NULL};
    
    s_vector* sus = s_vector_init();
    vector* matches = vector_init();

    add_subst(sus, "x___", 0, NULL);
    add_subst(sus, "y___", 3, "a", "b", "c");
    vector_push_back(matches, create_match(3, sus));
    s_vector_clear(sus);

    add_subst(sus, "x___", 1, "a");
    add_subst(sus, "y___", 2, "b", "c");
    vector_push_back(matches, create_match(3, sus));
    s_vector_clear(sus);

    add_subst(sus, "x___", 2, "a", "b");
    add_subst(sus, "y___", 1, "c");
    vector_push_back(matches, create_match(3, sus));
    s_vector_clear(sus);

    add_subst(sus, "x___", 3, "a", "b", "c");
    add_subst(sus, "y___", 0, NULL);
    vector_push_back(matches, create_match(3, sus));
    s_vector_clear(sus);

    IHCT_ASSERT(test_net(patterns, "f[a, b, c]", matches, false) == 0);
}

int main(int argc, char **argv) {
    return IHCT_RUN(argc, argv);
}