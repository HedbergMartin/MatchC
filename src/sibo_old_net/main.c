#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "list.h"
#include "discriminationNet.h"
#include <time.h>

/*struct parameters {
    bool isVar;
    bool isSeqVar;
    bool isFunc; //!Currently not implemented
    int value; //only used if isVar is false
    char* name;
    //enum restriction -- I'll see if I can be bothered, basically Integer or alike
};

struct function {
    char* name;
    int arity;
    struct parameters* params;
};*/

struct patternFunc {
    int open;   //!Is this needed?
    int close;  //!Is this needed?
    int arity;
    struct function* func;
    struct list* params;
    struct patternFunc* child;
    struct patternFunc* parent;
    struct patternFunc* sibling;
};

void freeFunction(void* funcV);
void printPatternFunc(struct patternFunc* top, int tabs);
void freePatternFunc(struct patternFunc* top);
struct parameters* parseParameter(char* buff, int start, int end);
struct patternFunc* openFunction(char* buff, int nameEstOffset, int nameEnd, struct patternFunc* new);
void closeFunction(char* buff, int nameEstOffset, int nameEnd, struct patternFunc* current);
void addParameter(char* buff, int nameEstOffset, int nameEnd, struct patternFunc* current);
struct patternFunc* parsePattern(char* buff, int buffLen);
void freeParams(void* e);
struct list* parseFunctionList(struct patternFunc* pf);
void printFunc(struct function* func);

//? Might be interesting to hash the function names, for faster the net.
int main(int argc, char* argv[]) {
    char* simple = "f[h[g], j_, p[], h, h, h, h, h, h, h, h]";
    clock_t t;

    t = clock();
    struct patternFunc* pf = parsePattern(simple, strlen(simple));
    t = clock() - t;

    double time_taken = ((double)t)/CLOCKS_PER_SEC;
  
    printf("took %f\n", time_taken);
    printPatternFunc(pf, 0);
    // return 0;
    // //char* simple = "func[x[p_, y, z__, m[]], 2, t_, p[s_, 1]] func[test_]";
    // char* simple = "func[x[p_, y, z__, m[]], 2, t_, p[s_, 1], t_, e_ , s_, t_, s_] func[d_, a_, c_, d_] func[d_, q_, 2, d_]";
    // //char* simple = "func[a_, b, x[]]";
    // struct patternFunc* pf = parsePattern(simple, strlen(simple));
    // printPatternFunc(pf, 0);

    // struct list* funcList = parseFunctionList(pf);

    // struct listElem* e = listNext(funcList, NULL);

    // struct discrNet* funcNet = netCreate("func");

    // while (e != NULL) {
    //     struct function* func = (struct function*)e->p;
    //     //printFunc(func);
        
    //     if (strcmp(func->name, "func") == 0) {
    //         netAddBranch(funcNet, func);
    //     } else {
    //         struct discrNet* net = netCreate(func->name);
    //         netAddBranch(net, func);
    //         netPrint(net);
    //         netRemove(net);
    //     }
    //     e = listNext(funcList, e);
    // }
    // netPrint(funcNet);
    // netRemove(funcNet);

    // freePatternFunc(pf);
    // listDelete(funcList);
}

void parseFunctionListHelper(struct list* funcList, struct patternFunc* current) {
    
    if (current == NULL) {
        return;
    }

    listAdd(funcList, current->func);

    parseFunctionListHelper(funcList, current->child);
    parseFunctionListHelper(funcList, current->sibling);
}

struct list* parseFunctionList(struct patternFunc* pf) {
    struct list* funcList = listCreate(freeFunction);
    parseFunctionListHelper(funcList, pf);

    return funcList;
}

/*void testParsePattern() {
    char* simple = "f[x]";
}*/

void freeFunction(void* funcV) {
    struct function* func = funcV;

    for (size_t i = 0; i < func->arity; i++) {

        if (func->params[i].isFunc == false) {
            free(func->params[i].name);
        }
    }

    free(func->name);
    free(func->params);
    free(func);
}

void printFunc(struct function* func) {
    fprintf(stdout, "%s[", func->name);

    for (size_t i = 0; i < func->arity; i++) {
        fprintf(stdout, "%s(", func->params[i].name);

        if (func->params[i].isVar) {
            fprintf(stdout, "V");
        }

        if (func->params[i].isSeqVar) {
            fprintf(stdout, ",S");
        }

        if (func->params[i].isFunc) {
            fprintf(stdout, ",F");
        }
        fprintf(stdout, ")");

        if (i != func->arity - 1) {
            fprintf(stdout, ",");
        }
        
    }

    fprintf(stdout, "]\n");
}

void printPatternFunc(struct patternFunc* top, int tabs) {

    if (top == NULL) {
        return;
    }

    for (size_t i = 0; i < tabs; i++) {
        fprintf(stdout, "\t");
    }
    
    printFunc(top->func);

    printPatternFunc(top->child, tabs + 1);

    printPatternFunc(top->sibling, tabs);

    return;
}

void freePatternFunc(struct patternFunc* top) {

    if (top == NULL) {
        return;
    }
    
    //Recursively frees all childs.
    if (top->child != NULL) {
        freePatternFunc(top->child);  
    }
    
    //Recursively frees all the siblings.
    if (top->sibling != NULL) {
        freePatternFunc(top->sibling);
    }
    
    //Frees itself when childs and siblings are freed.
    //freeFunction(top->func);
    listDelete(top->params);
    free(top);
    return;
}

struct parameters* parseParameter(char* buff, int start, int end) {

    if (end - start <= 0) {
        fprintf(stderr, "unhandled error on 0 or negative name len\n");
        return NULL;
    }

    struct parameters* param = calloc(1, sizeof(struct parameters));

    char* name = calloc(1, 1 + (end - start)); //Maybe using calloc is faster than malloc and memset...?
    int written = 0;

    for (size_t i = start; i < end; i++) {
        
        if (buff[i] == ',' || buff[i] == '[') {
            fprintf(stderr, "unexpected [ or , in name at pos: %ld\n", i);
        }

        if (buff[i] == ' ' && written == 0) { //Skips initial whitespaces
            continue;
        } else {
            name[written] = buff[i];

            //! Note that "x_ _" is currently accepted as a sequence variable.
            if (name[written] == '_') { //Checks if name ends with underscore (ignoring whitespaces)

                if (param->isVar) {
                    param->isSeqVar = true;
                }
                param->isVar = true;
                
            } else if (param->isVar && name[written] != ' ') { 
                param->isVar = false;
                param->isSeqVar = false;
            }
            written += 1;
        }
    }

    if (buff[end] == '[') {
        param->isFunc = true;
    }

    if (written == 0) {
        fprintf(stderr, "only whitespaces found in name, unhandled error\n");
        free(name);
        free(param);
        return NULL;
    }
    param->name = name;

}

struct patternFunc* openFunction(char* buff, int nameEstOffset, int nameEnd, struct patternFunc* new) {
    new->params = listCreate(freeParams);
    //new->arity = 1;

    //char* name = parseName(buff, nameEnd - nameEstOffset, nameEnd, NULL, NULL);
    fprintf(stderr, "%d\n", nameEstOffset);
    struct parameters* param = parseParameter(buff, nameEnd - nameEstOffset, nameEnd);

    if (param != NULL) {
        struct function* func = calloc(1, sizeof(struct function));
        func->name = param->name;
        new->func = func;
        
        if (new->parent != NULL) {
            new->parent->arity += 1;
            listAdd(new->parent->params, (void*)param);
        } else {
            free(param);
        }
    }

    return new;
}

void closeFunction(char* buff, int nameEstOffset, int nameEnd, struct patternFunc* current) {

    struct parameters* param = parseParameter(buff, nameEnd - nameEstOffset, nameEnd);

    /*if (current->arity == 1 && param == NULL) {  //Error handling for functions with 0 arity
        current->arity = 0;
    } */
    
    if (param != NULL) {
        current->arity += 1;
        listAdd(current->params, (void*)param);
    }

    //Copying the list to an array
    struct parameters* params = calloc(current->arity, sizeof(struct parameters));
    struct listElem* next = listNext(current->params, NULL);

    /*if (next == NULL) {
        current->arity = 0;
    }*/

    for (size_t i = 0; i < current->arity; i++) {

        if (next == NULL) {
            fprintf(stderr, "ERROR: Parameters list has less parameters than arity for some reason\n");
            break;
        }

        struct parameters *p = (struct parameters*)(next->p);
        memcpy(params + i, p, sizeof(struct parameters));
        next = listNext(current->params, next);
    }
    
    current->func->params = params;
    current->func->arity = current->arity;
    
}

//!What happens on f[x_, g[], y_]? how is "]," handled?
//Probably easiest to have a boolean in parsePattern, lastWasFunc
void addParameter(char* buff, int nameEstOffset, int nameEnd, struct patternFunc* current) {

    struct parameters* param = parseParameter(buff, nameEnd - nameEstOffset, nameEnd);

    if (param != NULL) {
        current->arity += 1;
        listAdd(current->params, (void*)param);
    }
}

//!I can optimize name reading by writing the name into a e.g. 256 buffer, then when I encounter a ',' '[' or ']' I put a \0 and restart the pointer. 
//!     This might also improve the parameter functions into one function.
//!How to deal with complete wildcards, e.g. just '_'?
//!Can I assume each function has at least arity 1? Because, arity is only increased at ',' so what about f[x_] - that would have arity 0...
struct patternFunc* parsePattern(char* buff, int buffLen) { //Do I even need to build this....??
    struct patternFunc* top = NULL;//calloc(1, sizeof(struct patternFunc)); //first top isn't used
    struct patternFunc* current = NULL;
    int nameEstOffset = 0;

    for (int i = 0; i < buffLen; i++) {
        
        if (buff[i] == '[') {       //Opening function
            struct patternFunc* child = calloc(1, sizeof(struct patternFunc));

            if (current == NULL) {  //If on the "top" layer without parents.
                child->sibling = top;
                top = child;
            } else {                //If child to another function
                child->sibling = current->child;
                current->child = child;
                child->parent = current;
            }
            current = child;
            current->open = i; 
            openFunction(buff, nameEstOffset, i, child);

            nameEstOffset = 0;
        } else if (buff[i] == ']') {//Closing function

            if (current == NULL) {
                fprintf(stderr, "] before a [\n");
                exit(1);
            }
            closeFunction(buff, nameEstOffset, i, current);

            current->close = i;
            current = current->parent;    

            nameEstOffset = 0;
        } else if (buff[i] == ',') {//Counting arity.

            if (current == NULL) {
                fprintf(stderr, ", before a [\n");
                exit(1);
            }

            addParameter(buff, nameEstOffset, i, current);
            
            nameEstOffset = 0;
        } else {
            nameEstOffset += 1;
        }
    }
    
    if (current != NULL) {
        fprintf(stderr, "Not same amount of [ as ]\n");
        exit(1);
    }

    return top;
    //!need to free stuff
}

void freeParams(void* e) {
    struct parameters* p = (struct parameters*)e;
    //free(p->name); //!Shouldn't be freed here since otherwise name will be lost when freeing the patternFunc
    free(p);
}



/*
char* parseName(char* buff, int start, int end, bool* isVar, bool* isSeqVar) {

    //Some error handling for sending NULLS on isVar and isSeqVar
    if (isVar == NULL) {
        bool notUsed = false;
        isVar = &notUsed;
    } else {
        *isVar = false;
    }

    //Some error handling for sending NULLS on isVar and isSeqVar
    if (isSeqVar == NULL) { 
        bool notUsed = false;
        isSeqVar = &notUsed;
    } else {
        *isSeqVar = false;
    }

    if (end - start <= 0) {
        fprintf(stderr, "unhandled error on 0 or negative name len\n");
        return NULL;
    }
    char* name = calloc(1, end - start); //Maybe using calloc is faster than malloc and memset...?
    int written = 0;
    
    for (size_t i = start; i < end; i++) {
        
        if (buff[i] == ',' || buff[i] == '[') {
            fprintf(stderr, "unexpected [ or , in name at pos: %ld\n", i);
        }

        if (buff[i] == ' ' && written == 0) { //Skips initial whitespaces
            continue;
        } else {
            name[written] = buff[i];
            written += 1;

            //! Note that "x_ _" is currently accepted as a sequence variable.
            if (!(*isVar) && name[written] == '_') { //Checks if name ends with underscore (ignoring whitespaces)

                if (*isVar) {
                    *isSeqVar = true;
                }
                *isVar = true;
                
            } else if (*isVar && name[written] != ' ') { 
                *isVar = false;
                *isSeqVar = false;
            }
        }
    }

    if (written == 0) {
        fprintf(stderr, "only whitespaces found in name, unhandled error\n");
        free(name);
        return NULL;
    }
    //memset(name + written, '\0', end - written); 

    return name;
}*/