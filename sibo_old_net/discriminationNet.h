struct parameters {
    bool isVar;
    bool isSeqVar; //Currently not used in discrNet
    bool isFunc;
    char* name;
};

struct function {
    char* name;
    int arity;
    struct parameters* params;
};

struct branchInfo {
    char* constOrFuncName;
    int variable;
    struct branch* b;
};

struct branch {
    struct branchInfo* branches;
    int branchesSize;
    struct list* funcs;
};

struct discrNet {
    char* name;
    struct branch* branch;
};

struct discrNet* netCreate(char* name);

void netAddBranch(struct discrNet* net, struct function* func);

void netRemove(struct discrNet* net);

void netPrint(struct discrNet* net);