#ifndef MC_MATCH_ENTRY
#define MC_MATCH_ENTRY

#include "subjectFlatterm.h"
#include "int_vector.h"
#include "vector.h"

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

match_entry* create_match(char* pattern, int_vector* sv, sub_arr_entry* s_arr, vector* idLookup);

void match_free(void* m);

#endif
