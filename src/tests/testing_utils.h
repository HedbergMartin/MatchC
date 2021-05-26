#ifndef MC_TESTING_UTILS
#define MC_TESTING_UTILS

#include "s_vector.h"
#include "d_net.h"
#include "vector.h"
#include "subjectFlatterm.h"
#include "ihct.h"
#include "match_entry.h"

#define INIT_MATCHER vector* sus = vector_init(); vector* matches = vector_init();
#define ADD_SUBST(from, amount, to...) add_subst(sus, from, amount, to);
#define REGISTER_MATCH vector_push_back(matches, create_ref_match(NULL, sus)); vector_clear(sus, NULL);
#define ASSERT_MATCH(patterns, subject, result) IHCT_ASSERT(test_net(patterns, subject, matches, false) == result); vector_free(sus, NULL); vector_free(matches, NULL);


match_entry* create_ref_match(char* pattern, vector* v);

void add_subst(vector* v, char* from, int len, ...);

int compare_subst(substitution* su, substitution* suRef, int debug);

int valid_match(match_entry* match, vector* refmatches, int debug);

int test_net(char* patterns[], char* subject, vector* refmatches, int debug);

void load_patterns(char* filename, d_net* net, double* parseTime, double* addTime );

void load_subjects(char* filename, subjectFlatterm** subjects, int subjectCount, double* parseTime, d_net* net);

void test_free(void* var);

#endif
