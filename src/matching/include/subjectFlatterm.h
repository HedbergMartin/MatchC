#ifndef SUBJECTFLATTERM_H
#define SUBJECTFLATTERM_H
#include "match_types.h"
#include "hash_table_linked.h"

typedef struct subjectFlatterm {
    char* symbol;
    int id;
    int fullNameId;
    struct subjectFlatterm* next;
    struct subjectFlatterm* skip;
    struct subjectFlatterm* parent;
    enum functype f_type;
    char** fullName;
}subjectFlatterm;

subjectFlatterm* parse_subject(char* subject, hash_table* symbolHt);
void print_subjectFlatterm(struct subjectFlatterm* sf);
void subjectFlatterm_free(struct subjectFlatterm* ft);

#endif