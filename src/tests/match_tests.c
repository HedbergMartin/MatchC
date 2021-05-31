#include "ihct.h"
#include "d_net.h"
#include "time.h"
#include "stdio.h"
#include "testing_utils.h"

IHCT_TEST(perf_test) {
    
    char* filenamePatterns = "../../../patterns.txt";
    char* filenameSubjects = "../../../subjects.txt";
    d_net* net = net_init();
    int subjectCount = 100000;

    double timeParsePattern = 0;
    double timeAddPattern = 0;
    double timeParseSubject = 0;
    double timeMatch = 0;

    clock_t matchStart;
    clock_t matchEnd;
 
    load_patterns(filenamePatterns, net, &timeParsePattern, &timeAddPattern);


    subjectFlatterm** subjects = malloc(sizeof(subjectFlatterm) * (subjectCount+1) );
    load_subjects(filenameSubjects, subjects, subjectCount, &timeParseSubject, net);
    int totalMatches = 0;
    for (int i = 0; i < subjectCount; i++) {
        //fprintf(stderr, "i: %d\n", i);
        //print_subjectFlatterm(subjects[i]);
        //printf("-----\nMatches:\n");
        matchStart = clock();
        match_set* matches = pattern_match_measure(net, subjects[i]);
        matchEnd = clock();
        timeMatch += (double)(matchEnd - matchStart) / CLOCKS_PER_SEC;

        totalMatches += matches_size(matches);

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

        // Crashes with empty match?
        match_set_free(matches);
    }

    fprintf(stderr, "timeParsePattern: %f\n", timeParsePattern);
    fprintf(stderr, "timeAddPattern: %f\n", timeAddPattern);
    fprintf(stderr, "timeParseSubject: %f\n", timeParseSubject);
    fprintf(stderr, "timeMatch: %f\n", timeMatch);
    fprintf(stderr, "totalMatches: %d\n", totalMatches);
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

    ADD_SUBST("x", 1, "g[h[]]");
    REGISTER_MATCH

    ASSERT_MATCH(patterns, "f[g[h[]]]", 0);
}

IHCT_TEST(variable_function_match_fail1) {
    char* patterns[] = {"f[x_]", NULL};

    INIT_MATCHER

    ADD_SUBST("x", 1, "g");
    REGISTER_MATCH

    ASSERT_MATCH(patterns, "f[g[h[]]]", 1);
}

IHCT_TEST(variable_function_match_fail2) {
    char* patterns[] = {"f[x_]", NULL};

    INIT_MATCHER

    ADD_SUBST("x", 1, "g[h]");
    REGISTER_MATCH

    ASSERT_MATCH(patterns, "f[g[h[]]]", 1);
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
    ADD_SUBST("y", 1, "c");fprintf(stderr, "%s[", current->symbol);
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

IHCT_TEST(double_x_diffrent_functions) {
    char* patterns[] = {"f[x_, x_]", NULL};

    INIT_MATCHER

    ASSERT_MATCH(patterns, "f[g[1], g[2]]", 0);
}


IHCT_TEST(double_x_same_functions) {
    char* patterns[] = {"f[x_, x_]", NULL};

    INIT_MATCHER

    ADD_SUBST("x", 1, "g[1]");
    REGISTER_MATCH

    ASSERT_MATCH(patterns, "f[g[1], g[1]]", 0);
}

IHCT_TEST(variable_function_match2) {
    char* patterns[] = {"f[x_]", NULL};

    INIT_MATCHER

    ASSERT_MATCH(patterns, "f[g, 1]", 0);
}

IHCT_TEST(sequence_mid_function) {
    char* patterns[] = {"f[x___]", NULL};

    INIT_MATCHER

    ADD_SUBST("x", 5, "g", "1", "h[a[]]", "2", "1");
    REGISTER_MATCH

    ASSERT_MATCH(patterns, "f[g, 1, h[a[]], 2, 1]", 0);
}


IHCT_TEST(const_test) {
    char* patterns[] = {"f[x_, p, x_]", NULL};

    INIT_MATCHER
    ADD_SUBST("x", 1, "t");
    REGISTER_MATCH

    ASSERT_MATCH(patterns, "f[t, p, t]", 0);
}


IHCT_TEST(nested_function_to_variable) {
    char* patterns[] = {"f[x_,y_,x_]", NULL};

    INIT_MATCHER

    ASSERT_MATCH(patterns, "f[G[a,G[b,c]],c,G[a,G[b,d]]]", 0);
}

IHCT_TEST(substitution_constraint) {
    char* patterns[] = {"f[x___,y___,x___]", NULL};

    INIT_MATCHER

    ADD_SUBST("x", -1, NULL);
    ADD_SUBST("y", 3, "a", "b", "c");
    REGISTER_MATCH

    ASSERT_MATCH(patterns, "f[a, b, c]", 0);
}

IHCT_TEST(sequence_match_multiple) {
    char* patterns[] = {"f[x___,y___,z___]", NULL};

    INIT_MATCHER

    char* func = "G[a, G[b, c]]";
    char* c = "c";

    //"falling" from x ---

    ADD_SUBST("x", 3, func, c, func);
    ADD_SUBST("y", -1, NULL);
    ADD_SUBST("z", -1, NULL);
    REGISTER_MATCH

    ADD_SUBST("x", 2, func, c);
    ADD_SUBST("y", 1, func);
    ADD_SUBST("z", -1, NULL);
    REGISTER_MATCH

    ADD_SUBST("x", 2, func, c);
    ADD_SUBST("y", -1, NULL);
    ADD_SUBST("z", 1, func);
    REGISTER_MATCH

    ADD_SUBST("x", 1, func);
    ADD_SUBST("y", 1, c);
    ADD_SUBST("z", 1, func);
    REGISTER_MATCH

    ADD_SUBST("x", 1, func);
    ADD_SUBST("y", -1, NULL);
    ADD_SUBST("z", 2, c, func);
    REGISTER_MATCH

    ADD_SUBST("x", 1, func);
    ADD_SUBST("y", 2, c, func);
    ADD_SUBST("z", -1, NULL);
    REGISTER_MATCH

    //x end ---

    //"falling" from y ---

    ADD_SUBST("x", -1, NULL);
    ADD_SUBST("y", 3, func, c, func);
    ADD_SUBST("z", -1, NULL);
    REGISTER_MATCH

    ADD_SUBST("x", -1, NULL);
    ADD_SUBST("y", 2, func, c);
    ADD_SUBST("z", 1, func);
    REGISTER_MATCH

    ADD_SUBST("x", -1, NULL);
    ADD_SUBST("y", 1, func);
    ADD_SUBST("z", 2, c, func);
    REGISTER_MATCH

    //y end ---

    ADD_SUBST("x", -1, NULL);
    ADD_SUBST("y", -1, NULL);
    ADD_SUBST("z", 3, func, c, func);
    REGISTER_MATCH

    ASSERT_MATCH(patterns, "f[G[a, G[b, c]], c, G[a, G[b, c]]]", 0);
}

IHCT_TEST(function_name_symbol_match) {
    char* patterns[] = {"f[x_,G[y_,x_]]", NULL};

    INIT_MATCHER

    ADD_SUBST("x", 1, "a");
    ADD_SUBST("y", 1, "b");
    REGISTER_MATCH

    ASSERT_MATCH(patterns, "f[a, G[b, a]]", 0);
}

IHCT_TEST(function_name_variable_match) {
    char* patterns[] = {"f[x_, g_[y_, x_]]", NULL};

    INIT_MATCHER

    ADD_SUBST("x", 1, "a");
    ADD_SUBST("g", 1, "t");
    ADD_SUBST("y", 1, "b");
    REGISTER_MATCH

    ASSERT_MATCH(patterns, "f[a, t[b, a]]", 0);
}

IHCT_TEST(sequence_empty) {
    char* patterns[] = {"f[x___,y___,z___]", NULL};

    INIT_MATCHER

    ADD_SUBST("x", -1, NULL);
    ADD_SUBST("y", -1, NULL);
    ADD_SUBST("z", -1, NULL);
    REGISTER_MATCH

    ASSERT_MATCH(patterns, "f[]", 0);
}

IHCT_TEST(sequence_single) {
    char* patterns[] = {"f[x___,y___,z___]", NULL};

    INIT_MATCHER

    ADD_SUBST("x", 1, "a");
    ADD_SUBST("y", -1, NULL);
    ADD_SUBST("z", -1, NULL);
    REGISTER_MATCH

    ADD_SUBST("x", -1, NULL);
    ADD_SUBST("y", 1, "a");
    ADD_SUBST("z", -1, NULL);
    REGISTER_MATCH

    ADD_SUBST("x", -1, NULL);
    ADD_SUBST("y", -1, NULL);
    ADD_SUBST("z", 1, "a");
    REGISTER_MATCH

    ASSERT_MATCH(patterns, "f[a]", 0);
}


int main(int argc, char **argv) {
    return IHCT_RUN(argc, argv);
}