#include "subst_tree.h"
#include <stdlib.h>
#include <memory.h>
#include <stdio.h>
#include <errno.h>

typedef struct st_branch {
    struct st_branch* parent;
    struct st_branch* child;
    struct st_branch* sibling;
} st_branch;

struct subst_tree {
    st_branch* branch;
};

subst_tree* init_subst_tree() {
    subst_tree* s = malloc(sizeof(subst_tree));
    if (!s) {
        perror("subst_tree");
        exit(1);
    }

    return s;
}
