#include "match_entry.h"

#include <stdio.h>
#include <memory.h>
#include <stdlib.h>

struct match_set {
    subjectFlatterm* owner;
    vector* matches; //TODO Make speciallized vector datatype
};

match_entry* create_match(char* pattern, sub_arr_entry* s_arr, flatterm* matchingFt, int depth, int matchCount) {
    match_entry* match = malloc(sizeof(match_entry));
    match->pattern = pattern;
    match->subst_amount = depth;
    match->matchCount = matchCount;
    match->substitutions = malloc(match->subst_amount * sizeof(substitution));
    term* current = flatterm_first(matchingFt);

    for (int i = 1; i < match->subst_amount + 1; i++) {

        if (current->m_type == MT_CONSTANT) {
            current = current->next;
            i -= 1;
            continue;
        } 
        substitution* sub = &(match->substitutions[i - 1]);
        sub->from = current->symbol;
        sub->len = s_arr[i].len;
        //fprintf(stderr, "\nFrom: %s, to - ", sub->from);

        
        //int from_id = int_vector_at(sv, i);
       // curr->from = (char*)vector_at(idLookup, from_id);
        //curr->len = s_arr[from_id].len;

        if (s_arr[i].to != NULL) {

            if (current->f_type == FT_PREFIX && current->m_type == MT_VARIABLE) {
                sub->to = &s_arr[i].to->symbol;
            } else {
                sub->to = s_arr[i].to->fullName;
            }
            //fprintf(stderr, "%s, ", *sub->to);
            
            /*subjectFlatterm* ft = s_arr[i].to;
            for (int g = 0; g < curr->len; g++) {
                // fprintf(stderr, "asd: %s, %s, %s\n", curr->to[g], ft->symbol, ft->fullName[0]);
                ft = ft->next;
            }*/
            
        } else {
            sub->to = NULL;
        }
        current = current->next;
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
