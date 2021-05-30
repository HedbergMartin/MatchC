#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "subjectFlatterm.h"
#include "hash_table_linked.h"
#include "vector.h"

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

struct subjectFlatterm* _parse_subject(char* subject, subjectFlatterm* parent, int* index, hash_table* subjectHt, hash_table* symbolHt, int* variableCount) {
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
                int id = hash_table_get_entry(symbolHt, name);

                if (id == -1) {
                    id = hash_table_insert_if_absent(subjectHt, name);
                }
                struct subjectFlatterm* current = new_subjectFlatterm(name);
                *variableCount += 1;
                current->id = id;
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
                    prev = _parse_subject(subject, current, index, subjectHt, symbolHt, variableCount);
                    current->skip = NULL;
                    nextIsSkip = true;
                    start = *index + 1;
                    continue;
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

void full_name_parsing(char** fullName, int index, subjectFlatterm* parent) {
    subjectFlatterm* current = parent;

    if (current->f_type != FT_PREFIX) {
        fprintf(stderr, "fullname parent should be prefix function!!\n");
        return;
    }

    
}

void add_fullname(subjectFlatterm* current, char* name, char** fullName, int* index, hash_table* subjectHt) {
    fullName[*index] = name;
    if (current->f_type == FT_PREFIX) {
        current->fullNameId = hash_table_insert_if_absent(subjectHt, name);
    } else {
        current->fullNameId = current->id;
    }
    current->fullName = &(fullName[*index]);
    (*index)++;
}

char* dive(subjectFlatterm* current, char** fullName, int* index, char* buffer, hash_table* subjectHt) {
    if (current->f_type == FT_PREFIX) {
        subjectFlatterm* ft = current->next;
        vector* v = vector_init();

        while (ft != current->skip) {
            vector_push_back(v, dive(ft, fullName, index, buffer, subjectHt));
            ft = ft->skip;
        }

        int pos = 0;
        int len = strlen(current->symbol);
        memcpy(buffer, current->symbol, len);
        pos += len;
        
        memcpy(&(buffer[pos]), "[", 1);
        pos += 1;

        int i = 0;
        ft = current->next;
        int amount_args = vector_size(v);
        (*index) = *index - amount_args + 1;
        while (ft != current->skip) {
            char* argsym = vector_at(v, i);
            add_fullname(ft, argsym, fullName, index, subjectHt);
            
            int len = strlen(argsym);
            memcpy(&(buffer[pos]), argsym, len);
            pos += len;

            i++;
            if (i != amount_args) {
                memcpy(&(buffer[pos]), ", ", 2);
                pos += 2;
            }

            ft = ft->skip;
        }
        (*index) = *index - amount_args - 1;
        
        memcpy(&(buffer[pos]), "]\0", 2);
        pos += 2;

        char* fullFuncName = malloc(pos * sizeof(char));
        memcpy(fullFuncName, buffer, pos);

        vector_free(v, NULL);

        return fullFuncName;

        // int len = full_name_func(current, current, buffer, 0);
        // char* fullName = malloc(len + 1);
        // memcpy(fullName, buffer, len);
        // fullName[len] = '\0';
        // first->fullName[index] = fullName;
        // current->fullNameId = hash_table_insert_if_absent(subjectHt, fullName, nextId);
        // current->fullName = &first->fullName[index];

    } else {
        return current->symbol;
    }
}

int full_name_func(subjectFlatterm* start, subjectFlatterm* parent, char* buffer, int pos) {
    subjectFlatterm* current = start;

    while (parent->skip != current) {
        int symbolLen = strlen(current->symbol);

        memcpy(&buffer[pos], current->symbol, symbolLen);
        pos += symbolLen;

        if (current->f_type == FT_PREFIX) {
            memcpy(&buffer[pos], "[", 1);
            pos += 1;
            pos = full_name_func(current->next, current, buffer, pos);
            memcpy(&buffer[pos], "]", 1);
            pos += 1;
            current = current->skip;
        } else {
            full_name_func(current->next, parent, buffer, pos);
            current = current->next;
        }

        if (parent->skip != current) {
            memcpy(&buffer[pos], ",", 1);
            pos += 1;
        }
        
    }
    return pos;
}

//Non function shit!!!
int full_name_init(char* subject, subjectFlatterm* current, subjectFlatterm* parent, subjectFlatterm* first, int index, hash_table* subjectHt, char* buffer) {

    if (current == NULL) {
        return index - 1;
    }

    if (current != parent->skip) {

        if (current->f_type != FT_PREFIX) {
            first->fullName[index] = current->symbol;
            current->fullNameId = current->id;
            current->fullName = &first->fullName[index];    
        }
        index = full_name_init(subject, current->skip, parent, first, index + 1, subjectHt, buffer);
    }
    
    if (current->f_type == FT_PREFIX) {
        int len = full_name_func(current, current, buffer, 0);    
        char* fullName = malloc(len + 1);
        memcpy(fullName, buffer, len);
        fullName[len] = '\0';
        first->fullName[index] = fullName;
        current->fullNameId = hash_table_insert_if_absent(subjectHt, fullName);
        current->fullName = &first->fullName[index];

        index = full_name_init(subject, current->next, current, first, index + 1, subjectHt, buffer);
    }

    return index;
        
    /*} else {
        
        first->fullName[index] = current->symbol;
        current->fullNameId = current->id;
        current->fullName = &first->fullName[index];
        *index += 1;
        full_name_init(subject, current->skip, first, index, subjectHt, nextId, buffer);
        
    }
    
    current = current->next;*/

}

/*void full_name_init(subjectFlatterm* parent) {
    subjectFlatterm;
    subjectFlatterm* end = current->skip;

    while (current->next != end) {

        current
    }
}*/

/*void full_name_init(char* subject, subjectFlatterm* first, int variableCount, hash_table* subjectHt, int* nextId) {
    int subjLen = strlen(subject);
    char* buffer = calloc(1, sizeof(char) * subjLen + 1);

    first->fullName = malloc(sizeof(char**) * variableCount);
    subjectFlatterm* current = first;

    for (int i = 0; i < variableCount; i++) {

        if (current->f_type == FT_PREFIX) {
            int len = full_name_func(current, current, buffer, 0);    

            char* fullName = malloc(len + 1);
            memcpy(fullName, buffer, len);
            fullName[len] = '\0';
            first->fullName[i] = fullName;
            current->fullNameId = hash_table_insert_if_absent(subjectHt, fullName, nextId);
        } else {
            first->fullName[i] = current->symbol;
            current->fullNameId = current->id;
        }
        current->fullName = &first->fullName[i];
        current = current->next;
    }

    free(buffer);
}*/

// symbolHt should be const here
subjectFlatterm* parse_subject(char* subject, hash_table* symbolHt) {
    
    int index = 0;
    int variableCount = 0;
    hash_table* subjectHt = hash_table_init(500);
    subjectFlatterm* first = _parse_subject(subject, NULL, &index, subjectHt, symbolHt, &variableCount);

    while (first->parent != NULL) {
        first = first->parent;
    }

    int subjLen = strlen(subject);
    char* buffer = malloc(sizeof(char) * subjLen * 2); //*2 for saftey
    first->fullName = malloc(sizeof(char**) * variableCount);
    //int full_name_init(char* subject, subjectFlatterm* current, subjectFlatterm* parent, subjectFlatterm* first, int index, hash_table* subjectHt, int* nextId, char* buffer) 
    int fullNameIndex = variableCount - 1;
    char* completeFunc = dive(first, first->fullName, &fullNameIndex, buffer, subjectHt);
    add_fullname(first, completeFunc, first->fullName, &fullNameIndex, subjectHt);
    //full_name_init(subject, first, first, first, 0, subjectHt, &nextId, buffer);

    free(buffer);

    // for (int i = 0; i < variableCount; i++) {
    //     fprintf(stderr, "%s\n", first->fullName[i]);
    // }
    
    hash_table_free(subjectHt);
    return first;
}

void print_subjectFlatterm(struct subjectFlatterm* sf) {
    struct subjectFlatterm* current = sf;

    printf("Next order: ");

    while (current != NULL) {
        printf("%s(%d)(%s(%d)), ", current->symbol, current->id, current->fullName[0], current->fullNameId);

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
    char** fullName = ft->fullName;

    while (ft != NULL) {
        struct subjectFlatterm* next = ft->next;
        free(ft->symbol);

        if (ft->f_type == FT_PREFIX) {
            free(ft->fullName[0]);
        }
        free(ft);
        ft = next;
    }
    free(fullName);
}