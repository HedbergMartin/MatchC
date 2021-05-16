#ifndef SUBJECTFLATTERM_H
#define SUBJECTFLATTERM_H

typedef struct subjectFlatterm {
    char* symbol;
    struct subjectFlatterm* next;
    struct subjectFlatterm* skip;
    struct subjectFlatterm* parent;
}subjectFlatterm;

struct subjectFlatterm* parse_subject(char* subject);
void print_subjectFlatterm(struct subjectFlatterm* sf);

#endif