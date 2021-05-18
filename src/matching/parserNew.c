#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "parser.h"

term* _parseInfix(const char str[], int *index, term* term1, char* name, term** parent);

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

char infixSymbols[] = {'+', '-', '/', '*'};
char* infixNames[] = {"PLUS", "MINUS", "DIV", "MULT"};
term* _parseInfixes(const char str[], int *index, term* term1, term** parent) {

    for (int i = 0; i < sizeof(infixSymbols); i++) {

        if (str[*index] == infixSymbols[i]) {
            return _parseInfix(str, index, term1, infixNames[i], parent);
        }
    }
}

term* _parseInfix(const char str[], int *index, term* term1, char* name, term** parent) {
    *index += 1;
    term* newParent = term_create(term1->prev, term1->parent);
    newParent->f_type = FT_PREFIX;
    newParent->argno = 2;
    newParent->m_type = MT_CONSTANT;
    newParent->next = term1;

    int nameLen = strlen(name);
    newParent->symbol = malloc(nameLen + 1);
    memcpy(newParent->symbol, name, nameLen);
    newParent->symbol[nameLen] = '\0';

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

    if (isInfix(str[*index])) {
        term2 = _parseInfixes(str, index, term2, parent);
    }

    if (newParent->parent != NULL) {
        newParent->parent->end = term2;
    }

    if (str[*index] == ']') {
        *parent = newParent->parent->parent;
    }
    
    return term2;
}



flatterm* flatify(term* first) {
    term* last = first;
    fprintf(stderr, "last: %s\n", last->symbol);

    //Backing up
    while (last->next != NULL) {        
        last = last->next;
        fprintf(stderr, "last: %s\n", last->symbol);
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
        current = _parseNextTerm(str, &i, prev, parent);

        if (first == NULL) {
            first = current;
        }

        switch(str[i]) {
            case '\0':
                doneReading = true;
                break;
            case '[':
                if (current != NULL) {
                    current->f_type = FT_PREFIX;
                    parent = current;
                }
                break;
            case ']':

                if (parent == NULL) {
                    fprintf(stderr, "Invalid ']' before '['\n");
                    return NULL;
                }
                if (current != NULL) {
                    parent->argno += 1;
                    parent->end = current;
                    current->end = current;
                } else {
                    parent->end = prev;
                }
                parent = parent->parent;
                break;
            case ',':

                if (parent == NULL) {
                    fprintf(stderr, "Invalid ',' before '['\n");
                    return NULL;
                }
                if (current != NULL) {
                    current->f_type = FT_NOTAFUNC;
                    parent->argno += 1;
                    current->end = current;
                }
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
            case '+': //Fallthrough
                //current = _parseInfix(str, &i, current, &parent);
                //break;
            case '-': //Fallthrough
                //current = _parseInfix(str, &i, current, &parent);
                //break;
            case '*': //Fallthrough
                //current = _parseInfix(str, &i, current, &parent);
                //break;
            case '/':

                if (current != NULL) {
                    current = _parseInfixes(str, &i, current, &parent);
                }
                break;
        }

        if (doneReading) {
            break;
        }
        
        if (current != NULL) {
            prev = current;
        }
        i += 1;
    }
    return flatify(first);
}