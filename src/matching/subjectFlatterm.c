#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "subjectFlatterm.h"

struct subjectFlatterm* new_subjectFlatterm(char* symbol) {
    struct subjectFlatterm* sf = calloc(1, sizeof(struct subjectFlatterm));
    sf->symbol = symbol;
    
    return sf;
}

char* parse_name(char* subject, int start, int end) {
    int range = end - start;

    if (range <= 0) {
        return NULL;
    }

    char* name = calloc(range + 1, sizeof(char));
    size_t written = 0;
    bool hasMidWordSpace = false;
    
    for (size_t i = start; i < end; i++) {

        if (subject[i] != ' ') {

            if (hasMidWordSpace) {
                free(name);
                return NULL;
            }
            name[written] = subject[i];
            written += 1;
        } else {

            if (written > 0) {
                hasMidWordSpace = true;
            }
        }
    }

    if (written == 0) {
        free(name);
        return NULL;
    }
    return name;
}

struct subjectFlatterm* _parse_subject(char* subject, subjectFlatterm* parent, int* index, hash_table* subject_ht, hash_table* constant_ht, int nextId) {
    int start = *index;
    int end = *index;

    struct subjectFlatterm* prev = parent;
    bool nextIsSkip = false;

    while (subject[*index] != '\0') {

        if (subject[*index] == ',' || subject[*index] == '[' || subject[*index] == ']') {
            end = *index;
            char* name = parse_name(subject, start, end);
            start = *index + 1;

            if (name != NULL) { 
                int id = get_entry(constant_ht, name);

                if (id == -1) {
                    id = insert_if_absent(subject_ht, name, id, &nextId);
                    nextId += 1;
                }
                struct subjectFlatterm* current = new_subjectFlatterm(name);
                current->id;
                current->parent = parent;

                if (prev != NULL) {
                    prev->next = current;
                    prev->skip = current;
                }

                if (nextIsSkip) {
                    prev->skip = current;
                    subjectFlatterm* parentTemp = prev->parent;

                    while (parentTemp != parent) {
                        parentTemp->skip = current;
                        parentTemp = parentTemp->parent;
                    }

                    if (parentTemp != NULL) {
                        parentTemp->skip = current;
                    }
                }


                if (subject[*index] == '[') {
                    current->f_type = FT_PREFIX;
                    prev = _parse_subject(subject, current, index);
                    current->skip = NULL;
                    nextIsSkip = true;
                    start = *index + 1;
                } else {
                    prev = current;
                }
                
            }
            
            if (subject[*index] == ']') {
                *index += 1;
                return prev;
            }
        }

        *index += 1;
    }
    return prev;
}

struct subjectFlatterm* parse_subject(char* subject, hash_table* subject_ht, hash_table* constant_ht) {
    int index = 0;
    subjectFlatterm* first = _parse_subject(subject, NULL, &index, subject_ht, constant_ht);

    //fprintf(stderr, "Subject: %s\n", subject);

    while (first->parent != NULL) {
        first = first->parent;
    }

    //first->skip = NULL;

    return first;
    /*int index = 0;
    int start = 0;
    int end = 0;

    struct subjectFlatterm* first = NULL;
    struct subjectFlatterm* prev = NULL;
    struct subjectFlatterm* skip = NULL;
    struct subjectFlatterm* parent = NULL;
    int skips = 0;
    
    while (subject[index] != '\0') {

        if (subject[index] == ',' || subject[index] == '[' || subject[index] == ']') {
            end = index;
            char* name = parse_name(subject, start, end);
            

            if (skip != NULL) {
                fprintf(stderr, "current skip: %s\n", skip->symbol);
            }
            
            start = index + 1;
            
            if (name != NULL) {
                struct subjectFlatterm* current = new_subjectFlatterm(name);
                fprintf(stderr, "name for current: %s\n", current->symbol);
                current->parent = parent;

                if (first == NULL) {
                    first = current;
                }

                if (prev != NULL) {
                    prev->next = current;
                }

                if (skips != 0) {

                    for (int i = 0; i < skips + 1; i++) {
                        fprintf(stderr, "Skip parent = %p\n", skip);
                        skip->skip = current;
                        skip = skip->parent;
                    }
                    skips = 0;
                } else if (skip != NULL) {
                    skip->skip = current;
                }

                skip = current;
                if (subject[index] == '[') {
                    skip = NULL;
                    parent = current;
                } 
                
                prev = current;
            }

            if (subject[index] == ']') {
                skips += 1;

                if (parent != NULL) {
                    parent = parent->parent;
                }
            }
        }
        index += 1;
    }
    return first;*/
}

void print_subjectFlatterm(struct subjectFlatterm* sf) {
    struct subjectFlatterm* current = sf;

    printf("Next order: ");

    while (current != NULL) {
        printf("%s, ", current->symbol);

        current = current->next;
    }

    printf("\nParent order: ");
    current = sf;
    while (current != NULL) {

        if (current->parent == NULL) {
            printf("%s, ", "?");
        } else {
            printf("%s, ", current->parent->symbol);
        }

        current = current->next;
    }

    printf("\nSkip order: ");
    current = sf;
    while (current != NULL) {

        if (current->skip == NULL) {
            printf("%s, ", "?");
        } else {
            printf("%s, ", current->skip->symbol);
        }

        current = current->next;
    }
    printf("\n");
}


void subjectFlatterm_free(struct subjectFlatterm* ft) {

    while (ft != NULL) {
        struct subjectFlatterm* next = ft->next;
        free(ft->symbol);
        free(ft);
        ft = next;
    }
}