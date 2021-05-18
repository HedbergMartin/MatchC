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

struct subjectFlatterm* parse_subject(char* subject) {
    int index = 0;
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
            start = index + 1;
            
            if (name != NULL) {
                struct subjectFlatterm* current = new_subjectFlatterm(name);
                current->parent = parent;

                if (first == NULL) {
                    first = current;
                }

                if (prev != NULL) {
                    prev->next = current;
                }

                if (skips != 0) {

                    for (int i = 0; i < skips + 1; i++) {
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
    return first;
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
