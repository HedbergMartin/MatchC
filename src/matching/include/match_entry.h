#ifndef MC_MATCH_ENTRY
#define MC_MATCH_ENTRY

#include "subjectFlatterm.h"
#include "s_vector.h"
#include "vector.h"

typedef struct substitution {
    char* from;
    subjectFlatterm* to;
    int len;
} substitution;

typedef struct match_entry {
    char* pattern;
    substitution* substitutions;
    int subst_amount;
} match_entry;

match_entry* create_match(char* pattern, s_vector* sv, sub_arr_entry* s_arr, vector* idLookup);

#endif
