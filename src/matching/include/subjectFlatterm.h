#ifndef SUBJECTFLATTERM_H
#define SUBJECTFLATTERM_H
#include "flatterm.h"

typedef struct subjectFlatterm {
    char* symbol;
    int id;
    struct subjectFlatterm* next;
    struct subjectFlatterm* skip;
    struct subjectFlatterm* parent;
    enum functype f_type;
}subjectFlatterm;

struct subjectFlatterm* parse_subject(char* subject, hash_table* subject_ht, hash_table* constant_ht);
void print_subjectFlatterm(struct subjectFlatterm* sf);
void subjectFlatterm_free(struct subjectFlatterm* ft);

#endif