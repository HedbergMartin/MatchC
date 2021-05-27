#include "match_entry.h"

#include <stdio.h>
#include <memory.h>
#include <stdlib.h>

match_entry* create_match(char* pattern, int_vector* sv, sub_arr_entry* s_arr, vector* idLookup) {
    match_entry* match = malloc(sizeof(match_entry));
    match->pattern = pattern;
    match->subst_amount = int_vector_size(sv);
    match->substitutions = malloc(match->subst_amount * sizeof(substitution));

    for (int i = 0; i < match->subst_amount; i++) {
        int index = int_vector_at(sv, i);
        substitution* curr = &(match->substitutions[i]);
        curr->from = (char*)vector_at(idLookup, index);
        curr->len = s_arr[index].len;
        if (curr->len != -1) {
            curr->to = malloc(curr->len * sizeof(char*));//s_arr[index].to;
            subjectFlatterm* ft = s_arr[index].to;
            for (int j = 0; j < curr->len; j++) {
                if (ft == NULL) {
                    fprintf(stderr, "FT IS NULL\n");
                }
                int len = strlen(ft->symbol) + 1;
                curr->to[j] = malloc(len * sizeof(char*));
                memcpy(curr->to[j], ft->symbol, len);
                ft = ft->skip;
            }
        } else {
            curr->to = NULL;
        }
    }

    return match;
}

void match_free(void* m) {
    match_entry* me = (match_entry*)m;
    for (int i = 0; i < me->subst_amount; i++) {
        substitution* curr = &(me->substitutions[i]);
        for (int j = 0; j < curr->len; j++) {
            free(curr->to[j]);
        }
        free(curr->to);
    }
    free(me->substitutions);
    free(me);
}
