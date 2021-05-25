#ifndef SUBJECTFLATTERM_H
#define SUBJECTFLATTERM_H
#include "flatterm.h"

typedef struct subjectFlatterm {
    char* symbol;
    struct subjectFlatterm* next;
    struct subjectFlatterm* skip;
    struct subjectFlatterm* parent;
    enum functype f_type;
}subjectFlatterm;

struct subjectFlatterm* parse_subject(char* subject);
void print_subjectFlatterm(struct subjectFlatterm* sf);
void subjectFlatterm_free(struct subjectFlatterm* ft);

#endif