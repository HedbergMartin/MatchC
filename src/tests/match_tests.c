#include "ihct.h"
#include "d_net.h"
#include "time.h"
#include "stdio.h"
#include "testing_utils.h"

IHCT_TEST(perf_test) {
    
    char* filenamePatterns = "../../../patterns.txt";
    char* filenameSubjects = "../../../subjects.txt";
    d_net* net = net_init();
    int subjectCount = 10000;

    double timeParsePattern = 0;
    double timeAddPattern = 0;
    double timeParseSubject = 0;
    double timeMatch = 0;

    clock_t matchStart;
    clock_t matchEnd;
 
    load_patterns(filenamePatterns, net, &timeParsePattern, &timeAddPattern);


    subjectFlatterm** subjects = malloc(sizeof(subjectFlatterm) * (subjectCount+1) );
    load_subjects(filenameSubjects, subjects, subjectCount, &timeParseSubject, net);

    for (int i = 0; i < subjectCount; i++) {
        //fprintf(stderr, "i: %d\n", i);
        //print_subjectFlatterm(subjects[i]);
        //printf("-----\nMatches:\n");
        matchStart = clock();
        match_set* matches = pattern_match_measure(net, subjects[i]);
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
        match_set_free(matches);
    }

    fprintf(stderr, "timeParsePattern: %f\n", timeParsePattern);
    fprintf(stderr, "timeAddPattern: %f\n", timeAddPattern);
    fprintf(stderr, "timeParseSubject: %f\n", timeParseSubject);
    fprintf(stderr, "timeMatch: %f\n", timeMatch);
    //vector_print_push_pop();
    //print_times();

    IHCT_ASSERT(0 == 0);
    net_free(net);

    free(subjects);
}

// Something is left un-freed...
IHCT_TEST(variable_match) {
    char* patterns[] = {"f[x_]", NULL};

    INIT_MATCHER
    
    ADD_SUBST("x", 1, "2");
    REGISTER_MATCH

    ASSERT_MATCH(patterns, "f[2]", 0);
}

IHCT_TEST(variable_repeating_match) {
    char* patterns[] = {"f[x_, x_]", NULL};

    INIT_MATCHER

    ADD_SUBST("x", 1, "2");
    REGISTER_MATCH

    ASSERT_MATCH(patterns, "f[2, 2]", 0);
}

IHCT_TEST(variable_repeating_not_match1) {
    char* patterns[] = {"f[x_, x_]", NULL};

    INIT_MATCHER

    ADD_SUBST("x", 1, "2");
    ADD_SUBST("x", 1, "3");
    REGISTER_MATCH

    ASSERT_MATCH(patterns, "f[2, 3]", 1);
}

IHCT_TEST(variable_repeating_not_match2) {
    char* patterns[] = {"f[x_, x_]", NULL};
    INIT_MATCHER

    ADD_SUBST("x", 1, "2");
    REGISTER_MATCH

    ASSERT_MATCH(patterns, "f[2, 3]", 1);
}

IHCT_TEST(variable_function_match) {
    char* patterns[] = {"f[x_]", NULL};

    INIT_MATCHER

    ADD_SUBST("x", 1, "g");
    REGISTER_MATCH

    ASSERT_MATCH(patterns, "f[g[h[]]]", 0);
}

IHCT_TEST(sequence_match) {
    char* patterns[] = {"f[x___, y___]", NULL};

    INIT_MATCHER

    ADD_SUBST("x", -1, NULL);
    ADD_SUBST("y", 3, "a", "b", "c");
    REGISTER_MATCH

    ADD_SUBST("x", 1, "a");
    ADD_SUBST("y", 2, "b", "c");
    REGISTER_MATCH

    ADD_SUBST("x", 2, "a", "b");
    ADD_SUBST("y", 1, "c");
    REGISTER_MATCH

    ADD_SUBST("x", 3, "a", "b", "c");
    ADD_SUBST("y", -1, NULL);
    REGISTER_MATCH

    ASSERT_MATCH(patterns, "f[a, b, c]", 0);
}

IHCT_TEST(middle_sequence_match) {
    char* patterns[] = {"f[x___, y_, x___, y_, x___]", NULL};

    INIT_MATCHER

    ADD_SUBST("x", 4, "a", "a", "a", "a");
    ADD_SUBST("y", 1, "b");
    REGISTER_MATCH

    ASSERT_MATCH(patterns, "f[a,a,a,a,b,a,a,a,a,b,a,a,a,a]", 0);
}

IHCT_TEST(end_pluses_sequence_match) {
    char* patterns[] = {"f[x__, y_, k_, g__]", NULL};

    INIT_MATCHER

    ADD_SUBST("x", 2, "a", "b");
    ADD_SUBST("y", 1, "c");
    ADD_SUBST("k", 1, "d");
    ADD_SUBST("g", 1, "e");
    REGISTER_MATCH

    ADD_SUBST("x", 1, "a");
    ADD_SUBST("y", 1, "b");
    ADD_SUBST("k", 1, "c");
    ADD_SUBST("g", 2, "d", "e");
    REGISTER_MATCH

    ASSERT_MATCH(patterns, "f[a, b, c, d, e]", 0);
}

IHCT_TEST(end_star_sequence_match) {
    char* patterns[] = {"f[x___, y_, k_, g___]", NULL};

    INIT_MATCHER

    ADD_SUBST("x", 3, "a", "b", "c");
    ADD_SUBST("y", 1, "d");
    ADD_SUBST("k", 1, "e");
    ADD_SUBST("g", -1, NULL);
    REGISTER_MATCH

    ADD_SUBST("x", 2, "a", "b");
    ADD_SUBST("y", 1, "c");
    ADD_SUBST("k", 1, "d");
    ADD_SUBST("g", 1, "e");
    REGISTER_MATCH

    ADD_SUBST("x", 1, "a");
    ADD_SUBST("y", 1, "b");
    ADD_SUBST("k", 1, "c");
    ADD_SUBST("g", 2, "d", "e");
    REGISTER_MATCH

    ADD_SUBST("x", -1, NULL);
    ADD_SUBST("y", 1, "a");
    ADD_SUBST("k", 1, "b");
    ADD_SUBST("g", 3, "c", "d", "e");
    REGISTER_MATCH

    ASSERT_MATCH(patterns, "f[a, b, c, d, e]", 0);
}


int main(int argc, char **argv) {
    return IHCT_RUN(argc, argv);
}