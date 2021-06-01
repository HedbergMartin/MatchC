#ifndef MC_MATCH_ENTRY
#define MC_MATCH_ENTRY

#include "subjectFlatterm.h"
#include "vector.h"
#include "net_branch.h"
#include "flatterm.h"

typedef struct match_set match_set;

typedef struct substitution {
    char* from;
    char** to;
    int len;
} substitution;

typedef struct match_entry {
    char* pattern;
    substitution* substitutions;
    int subst_amount;
} match_entry;

typedef struct sub_arr_entry {
    subjectFlatterm* to;
    int len;
} sub_arr_entry;

match_entry* create_match(char* pattern, sub_arr_entry* s_arr, flatterm* matchingFt, int depth);

match_set* create_match_set(subjectFlatterm* ft, vector* matches);

size_t matches_size(match_set* ms);

match_entry* get_match(match_set* ms, size_t index);

void match_set_free(match_set* ms);

#endif
