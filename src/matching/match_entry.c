#include "match_entry.h"

match_entry* create_match(char* pattern, s_vector* sv, sub_arr_entry* s_arr, vector* idLookup) {
    match_entry* match = malloc(sizeof(match_entry));
    match->pattern = pattern;
    match->subst_amount = s_vector_size(sv);
    match->substitutions = malloc(match->subst_amount * sizeof(substitution));

    for (int i = 0; i < match->subst_amount; i++) {
        int index = s_vector_at(sv, i);
        match->substitutions[i].from = (char*)vector_at(idLookup, index);
        match->substitutions[i].to = s_arr[index].to;
        match->substitutions[i].len = s_arr[index].len;
    }

    return match;
}
