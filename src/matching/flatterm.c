#include "flatterm.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <errno.h>

struct flatterm {
    term* first;
    term* end;
};

flatterm* flatterm_init() {
    flatterm* ft = calloc(1, sizeof(flatterm));
    if (!ft) {
        perror("flatterm");
        exit(errno);
    }

    return ft;
}

flatterm* flatterm_init_complete(term* first, term* end) {
    flatterm* ft = calloc(1, sizeof(flatterm));
    if (!ft) {
        perror("flatterm");
        exit(errno);
    }

    ft->first = first;
    ft->end = end;
    return ft;
}

term* flatterm_push_back(flatterm* ft) {
    term* t = calloc(1, sizeof(term));
    if (!t) {
        perror("ft_add");
        exit(errno);
    }

	t->m_type = -1;
	t->f_type = -1;
	t->symbol = NULL;
    t->end = t;

    if (ft->first == NULL) {
        ft->first = t;
        ft->end = t;
    } else {
        ft->end->next = t;
        t->prev = ft->end;
        // t->next = NULL; Implicit by calloc
        ft->end = t;
    }

    return t;
}

term* flatterm_first(flatterm* ft) {
    return ft->first;
}

term* flatterm_end(flatterm* ft) {
    return ft->end;
}

void flatterm_free(flatterm* ft) {
    term* t = flatterm_first(ft);
    while (t) {
        term* next = t->next;
        free(t->symbol);
        free(t);
        t = next;
    }

    free(ft);
}

void flatterm_print_debug(flatterm* ft) {
    term* t = flatterm_first(ft);
    while (t) {
        //printf("Symbol: %s, type: %s, matching: %s, end-sym: %s\n", t->symbol, (t->f_type ? "prefix" : "symbol"), (t->m_type ? "variable" : "constant"), t->end->symbol);
        fprintf(stderr, "Symbol: %s,", t->symbol);
        fprintf(stderr, "type: %s, ", (t->f_type ? "prefix" : "symbol"));
        fprintf(stderr, "matching: %s, ", (t->m_type ? "variable" : "constant"));
        fprintf(stderr, "end-sym: %s\n", t->end->symbol);
        t = t->next;
    }
}


void _flatterm_func_print(term* current, term* parent) {
    bool prevWasVar = false;

    while (current != NULL && (parent == NULL || parent->end->next != current)) {
        
        if (current->f_type == FT_PREFIX) {

            switch(current->m_type) {
            case MT_CONSTANT:
                fprintf(stderr, "%s[", current->symbol);
            case MT_VARIABLE:
                fprintf(stderr, "%s_[", current->symbol);
            }

            
            _flatterm_func_print(current->next, current);
            fprintf(stderr, "]");
            current = current->end->next;
        } else {

            if (prevWasVar == true) {
                fprintf(stderr, ", ");
            }

            switch(current->m_type) {
            case MT_CONSTANT:
                fprintf(stderr, "%s", current->symbol);
                break;
            case MT_VARIABLE:
                fprintf(stderr, "%s_", current->symbol);
                break;
            case MT_SEQUENCE:
                fprintf(stderr, "%s__", current->symbol);
                break;
            case MT_STAR:
                fprintf(stderr, "%s___", current->symbol);
                break;
            }
            prevWasVar = true;
            current = current->next;
        }
    }
}

void flatterm_print(flatterm* ft) {
    term* t = flatterm_first(ft);

    _flatterm_func_print(t, NULL);
}