struct listElem {
    struct listElem* next;
    struct listElem* prev;
    void* p;
};

struct list {
    struct listElem* first;
    struct listElem* last;
    void (*free_elem)(void* e);
};

//Should probably have used a listPtr but got lazy

struct list* listCreate(void (*free_elem)(void* e));

void listRemove(struct list* l, struct listElem* e);

void listAdd(struct list* l, void* val);

struct listElem* listNext(struct list* l, struct listElem* e);

void listDelete(struct list* l);