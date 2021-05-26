#ifndef MC_TESTING_UTILS
#define MC_TESTING_UTILS

#include "s_vector.h"
#include "d_net.h"
#include "vector.h"
#include "subjectFlatterm.h"
#include "ihct.h"

#define INIT_MATCHER s_vector* sus = s_vector_init(); vector* matches = vector_init();
#define ADD_SUBST(from, amount, to...) add_subst(sus, from, amount, to);
#define REGISTER_MATCH vector_push_back(matches, create_match(3, sus)); s_vector_clear(sus);
#define ASSERT_MATCH(patterns, subject, result) IHCT_ASSERT(test_net(patterns, subject, matches, false) == result); s_vector_free(sus); vector_free(matches, NULL);


net_match* create_match(int id, s_vector* v);

void add_subst(s_vector* v, char* from, int len, ...);

int compare_subst(s_entry* su, s_entry* suRef, int debug);

int valid_match(net_match* match, vector* refmatches, int debug);

int test_net(char* patterns[], char* subject, vector* refmatches, int debug);

void load_patterns(char* filename, d_net* net, double* parseTime, double* addTime );

void load_subjects(char* filename, subjectFlatterm** subjects, int subjectCount, double* parseTime);

void test_free(void* var);

#endif
