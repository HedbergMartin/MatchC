#include "match_entry.h"

#include <stdio.h>
#include <memory.h>
#include <stdlib.h>

struct match_set {
    subjectFlatterm* owner;
    vector* matches; //TODO Make speciallized vector datatype
};

match_entry* create_match(char* pattern, int_vector* sv, sub_arr_entry* s_arr, vector* idLookup) {
    match_entry* match = malloc(sizeof(match_entry));
    match->pattern = pattern;
    match->subst_amount = int_vector_size(sv);
    match->substitutions = malloc(match->subst_amount * sizeof(substitution));

    for (int i = 0; i < match->subst_amount; i++) {
        substitution* curr = &(match->substitutions[i]);
        int from_id = int_vector_at(sv, i);
        curr->from = (char*)vector_at(idLookup, from_id);
        curr->len = s_arr[from_id].len;

        if (s_arr[from_id].to != NULL) {
            curr->to = s_arr[from_id].to->fullName;
            subjectFlatterm* ft = s_arr[from_id].to;

            for (int g = 0; g < curr->len; g++) {
                // fprintf(stderr, "asd: %s, %s, %s\n", curr->to[g], ft->symbol, ft->fullName[0]);
                ft = ft->next;
            }
            
        } else {
            curr->to = NULL;
        }
        
    }

    return match;
}

void match_free(void* m) {
    match_entry* me = (match_entry*)m;
    free(me->substitutions);
    free(me);
}

match_set* create_match_set(subjectFlatterm* ft, vector* matches) {
    match_set* ms = malloc(sizeof(match_set));
    ms->owner = ft;
    ms->matches = matches;

    return ms;
}

size_t matches_size(match_set* ms) {
    return vector_size(ms->matches);
}

match_entry* get_match(match_set* ms, size_t index) {
    return (match_entry*)vector_at(ms->matches, index);
}

void match_set_free(match_set* ms) {
    vector_free(ms->matches, match_free);
    subjectFlatterm_free(ms->owner);
    free(ms);
}
