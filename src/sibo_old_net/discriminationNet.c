#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "list.h"
#include "discriminationNet.h"

struct mapping {
    char* name;
    int id;
};

/**
 *  NOTE: name is not freed here since it's referenced in other places as well
 **/
void freeMapping(void* mapping) {
    struct mapping* map = (struct mapping*) mapping;
    free(map);
}

struct branch* createBranch() {
    struct branch* branch = calloc(1, sizeof(struct branch));
    branch->branches = calloc(2, sizeof(struct branchInfo));
    branch->branchesSize = 2;

    return branch;
}


struct discrNet* netCreate(char* name) {
    struct discrNet* net = calloc(1, sizeof(struct discrNet));
    net->name = name;
    net->branch = createBranch();

    return net;
}

/**
 * Match anything = 0
 * Match constant or function = -1 and char*
 * Match earlier variable >= 2
 * */

/**
 * This should definitely have used a hashmap but I got lazy and short on time
 **/
int findVarMapping(struct list *varMap, char* name, int* next) {
    struct listElem* elem = listNext(varMap, NULL);

    while (elem != NULL) {
        struct mapping* map = (struct mapping*)elem->p;
        
        if (strcmp(map->name, name) == 0) {
            return map->id;
        }
        elem = listNext(varMap, elem);
    }

    struct mapping* map = calloc(1, sizeof(struct mapping));
    map->name = name;
    map->id = *next;
    *next += 1;
    listAdd(varMap, map);
    
    return 0;
}

/**
 * Might seem ineffective but I don't know how many patterns might be added at init
 * But then the matching should be as fast as possible, thus I want it in arrays
 **/
void growBranch(struct branch* parent) {

    struct branchInfo* newBranches = calloc(parent->branchesSize * 2, sizeof(struct branchInfo));
    memcpy(newBranches, parent->branches, parent->branchesSize * sizeof(struct branchInfo)); //! This severly lacks error handling...
    parent->branchesSize = parent->branchesSize * 2;

    free(parent->branches);

    parent->branches = newBranches;
}

void addFuncToBranch(struct function* func, struct branch* branch) {

    if (branch->funcs == NULL) {
        branch->funcs = listCreate(NULL); //Can be NULL since I don't want the funcs to be removed from this list (at least yet)
    }

    listAdd(branch->funcs, func);
}

/**
 * Give each unique variable an int (through using the list, a hashmap would've been faster but I'm lazy atm)
 * If variable already has link, set branch depend (how does this happen efficiently?)
 * 
 * What makes variable unique, if it's a constant or function, or what other positions it's in
 * 
 * Growing the branches same as arrayLists with 2*n is probably smarter
 **/
void netAddBranch(struct discrNet* net, struct function* func) {

    if (strcmp(func->name, net->name) != 0) {
        fprintf(stderr, "Unhandled error if net and func is not same\n");
        return;
    }
    
    struct list* varMapping = listCreate(freeMapping);
    struct branch* branch = net->branch;
    int next = 1;

    for (size_t i = 0; i < func->arity; i++) {
        struct parameters param = func->params[i];
        
        if (param.isVar) {
            int mapping = findVarMapping(varMapping, param.name, &next);
            
            for (size_t i = 0; i < branch->branchesSize; i++) {
                
                if (branch->branches[i].b == NULL) { //Further branches aren't used
                    branch->branches[i].variable = mapping;
                    branch->branches[i].b = createBranch();
                    
                    if (i == branch->branchesSize - 1) {
                        growBranch(branch);
                    }
                    branch = branch->branches[i].b;
                    break;
                } else if (branch->branches[i].variable == mapping) {
                    branch = branch->branches[i].b;
                    break;
                }
            }
            
        } else {
            
            for (size_t i = 0; i < branch->branchesSize; i++) {

                if (branch->branches[i].b == NULL) { //Further branches aren't used
                    branch->branches[i].variable = -1;
                    branch->branches[i].constOrFuncName = param.name;
                    branch->branches[i].b = createBranch();
                    
                    if (i == branch->branchesSize - 1) {
                        growBranch(branch);
                    }
                    branch = branch->branches[i].b;
                    break;
                } else if (branch->branches[i].variable == -1) {

                    if (strcmp(branch->branches[i].constOrFuncName, param.name) == 0) {
                        branch = branch->branches[i].b;
                        break;
                    }
                }
            }

        }
        
        addFuncToBranch(func, branch);
    }
    
    listDelete(varMapping);
}

static void printNetHelper(struct branch* branch, int chars) {
    int tempChars = chars;

    for (size_t i = 0; i < branch->branchesSize; i++) {
        
        if (branch->branches[i].b != NULL) {

            if (branch->branches[i].variable >= 0) {
                fprintf(stdout, ",(%d)", branch->branches[i].variable);
                tempChars += 4; //This doesn't work if more than 9 variables
            } else {
                fprintf(stdout, ",%s", branch->branches[i].constOrFuncName);
                tempChars += strlen(branch->branches[i].constOrFuncName) + 1;
            }
            
            printNetHelper(branch->branches[i].b, tempChars);
        }

        if (i < branch->branchesSize - 1 && branch->branches[i+1].b != NULL) {
            fprintf(stdout, "\n");

            for (size_t i = 0; i < chars; i++) {
                fprintf(stdout, " ");
            }
        }
        
    }
}

void netPrint(struct discrNet* net) {
    fprintf(stdout, "Printing net for: %s\n", net->name);
    printNetHelper(net->branch, 0);
    fprintf(stdout, "\n");
}

static void netBranchRemover(struct branch* branch) {

    for (size_t i = 0; i < branch->branchesSize; i++) {
        
        if (branch->branches[i].b != NULL) {
            netBranchRemover(branch->branches[i].b);
        }
    }
    free(branch->branches);
    
    if (branch->funcs != NULL) {
        listDelete(branch->funcs);
    }

    free(branch);
}


void netRemove(struct discrNet* net) {
    
    netBranchRemover(net->branch);
    free(net);
}