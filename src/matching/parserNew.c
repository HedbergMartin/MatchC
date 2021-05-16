#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "parser.h"

bool isAcceptedCharacter(char c) {
	if ((c >= 'A' && c <= 'Z') || 
		(c >= 'a' && c <= 'z') || 
		(c >= '0' && c <= '9') || 
		c == '_') {
		return true;
	}

	return false;
}

bool isInfix(char c) {
    if (c == '+' || c == '-' ||
        c == '/' || c == '*') {
        return true;
    }
    return false;
}

bool isSpecialCharacter(char c) {
	if (c == '[' || c == ']' ||
        c == ',' || c == ':' ||
        isInfix(c) || 
        c == '\0') {
		return true;
	}

	return false;
}

char* parsePatternName(const char str[], int start, int end, term* t) {
    int range = end - start;

    if (range <= 0) {
        return NULL;
    }

    char* name = calloc(range + 1, sizeof(char));
    size_t written = 0;
    bool hasMidWordSpace = false;
    int trailing_ = 0;
    
    for (size_t i = start; i < end; i++) {

        if (isAcceptedCharacter(str[i])) {

            if (hasMidWordSpace) {
                free(name);
                return NULL;
            }
            name[written] = str[i];
            written += 1;

            if (str[i] == '_') {
                trailing_ += 1;
            } else {
				trailing_ = 0;
			}
        } else {

            if (written > 0 && str[i] == ' ') {
                hasMidWordSpace = true;
            }
        }
    }

    if (written == 0) {
        free(name);
        return NULL;
    }
    t->m_type = trailing_;
    return name;
}

term* term_create(term* prev, term* parent) {
	term* new = calloc(1, sizeof(struct term));
	if (new == NULL) {
		perror("term");
		exit(1);
	}

	new->m_type = 0;
	new->f_type = 0;
    new->prev = prev;
    new->parent = parent;

    if (prev != NULL) {
        prev->next = new;
    }

	return new;
}

term* _parseNextTerm(const char str[], int *index, term* prev, term* parent) {
    int nameStart = *index;

    while (!isSpecialCharacter(str[*index])) {
        *index += 1;
    }
    term* t = term_create(prev, parent);
    int nameEnd = *index;

    t->symbol = parsePatternName(str, nameStart, nameEnd, t);

    if (t->symbol == NULL) {
        prev->next = NULL;
        free(t);
        return NULL;
    }

    return t;
}

term* _parseInfix(const char str[], int *index, term* term1, char* name) {
    *index += 1;
    term* newParent = term_create(term1->prev, term1->parent);
    newParent->f_type = FT_PREFIX;
    newParent->argno = 2;
    newParent->m_type = MT_CONSTANT;
    //newParent->prev = term1->prev;
    newParent->next = term1;

    int nameLen = strlen(name);
    newParent->symbol = malloc(nameLen + 1);
    memcpy(newParent->symbol, name, nameLen);
    newParent->symbol[nameLen] = '\0';

    /*if (newParent->prev != NULL) {
        newParent->prev->next = newParent;
    }*/
    term* term2 = _parseNextTerm(str, index , term1, newParent);

    //!Need to handle this error
    if (term2 == NULL) { 
        fprintf(stderr, "Infix unhandled error!!\n");
    }
    
    term1->prev = newParent;
    term1->parent = newParent;
    term1->f_type = FT_NOTAFUNC;
    term2->f_type = FT_NOTAFUNC;
    term1->end = term1;
    term2->end = term2;
    newParent->end = term2;
    fprintf(stderr, "%s and %s\n", term1->symbol, term2->symbol);

    /*if (isInfix(str[*index])) { NOT WORKING ATM
        term2 = _parseInfix(str, index, term2, "test");
    }*/
    return term2;
}



flatterm* flatify(term* first) {
    term* last = first;

    //fprintf(stderr, "%d\n", first);
    //Backing up
    while (last->next != NULL) {        
        last = last->next;
    }
    flatterm* ft = flatterm_init_complete(first, last);
    return ft;
}

flatterm* parsePattern(const char str[]) {
    int i = 0;
    term* parent = NULL;
    term* prev = NULL;
    term* current = NULL;
    term* first = NULL;
    char* infixName = NULL;
    bool doneReading = false;
    bool wasInfixed = false;
    int args = 0;

    while (str[i] != '\0') {
        fprintf(stderr, "str: %s\n", &str[i] );
        current = _parseNextTerm(str, &i, prev, parent);

        if (current == NULL && str[i] != '\0' && str[i] != ':') { //Invalid term
            i += 1;
            continue;
        }
        if (first == NULL) {
            first = current;
        }


        fprintf(stderr, "Switcher: %c\n", str[i]);


        switch(str[i]) {
            case '\0':
                doneReading = true;
                break;
            case '[':
                current->f_type = FT_PREFIX;
                parent = current;
                break;
            case ']':

                if (parent == NULL) {
                    fprintf(stderr, "Invalid ']' before '['\n");
                    return NULL;
                }
                parent->argno += 1;
                parent->end = current;
                parent = parent->parent;
                current->end = current;
                break;
            case ',':

                if (parent == NULL) {
                    fprintf(stderr, "Invalid ',' before '['\n");
                    return NULL;
                }
                current->f_type = FT_NOTAFUNC;
                parent->argno += 1;
                current->end = current;
                break;
            case ':':
                
                if (parent != NULL) {
                    fprintf(stderr, ":= at non ending position\n");
                    return NULL;
                }
                if (str[i + 1] != '=') {
                    fprintf(stderr, "missing '=' after ':'\n");
                    return NULL;
                }
                doneReading = true;
                break;
            case '+':
                current = _parseInfix(str, &i, current, "PLUS");
                wasInfixed = true;
                break;
            case '-': 
                current = _parseInfix(str, &i, current, "MINUS");
                wasInfixed = true;
                break;
            case '*': 
                current = _parseInfix(str, &i, current, "MULT");
                wasInfixed = true;
                break;
            case '/':
                current = _parseInfix(str, &i, current, "DIV");
                wasInfixed = true;
                break;
        }

        if (doneReading) {
            break;
        }

        if (str[i] == ']' && wasInfixed) {
            parent = current->parent->parent->parent;

            if (current->parent->parent != NULL) {
                current->parent->parent->end = current;
            }
        }
        
        wasInfixed = false;
        
        prev = current;
        i += 1;
    }
    return flatify(first);
}

/*
int _parseFunction(const char str[], term* parent, int start) {
    int i = 0;
    int nameStart = start;
    int nameEnd = start;
    term* prev = parent;

    while (str[i] != '\0') {

        while (!isSpecialCharacter(str[i])) {
            i += 1;
        }
        nameEnd = i;
        term* t = term_create(prev, parent);

        t->symbol = parse_name(str, nameStart, nameEnd);

        if (t->symbol == NULL) {
            fprintf(stderr, "Invalid name for term\n");
        }

        switch(str[i]) {
            case '[':
                i = _parseFunction(str, t, i + 1);
                
                if (parent != NULL) {
                    parent->end = t;
                }
                break;
            case ']':
                return i + 1;
            case ',':
                t->end = t;
                break;
            case '+': //fallthrough
                
            case '-': //fallthrough
            case '*': //fallthrough
            case '/': //fallthrough

                break;
        }
    }
    fprintf(stderr, "Invalid ending to function\n");
}*/