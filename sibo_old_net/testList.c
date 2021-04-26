#include <stdlib.h>
#include <stdio.h>
#include "list.h"

void remElem(void* a) {
    return;
}

struct list* setupList() {
    return listCreate(remElem);
}

void testAdd() {
    struct list* l = setupList();
    char a = '1';
    //char* a = malloc(1);

    listAdd(l, &a);
    struct listElem* e = listNext(l, NULL);
    fprintf(stderr, "char: %c\n", *((char*)(e->p)));
    listDelete(l);
}

void testDelete() {

}


void testRemove() {

}

void testNext() {
    struct list* l = setupList();
    char a = '1';
    char b = '2';
    //char* a = malloc(1);

    listAdd(l, &a);
    listAdd(l, &b);
    struct listElem* e = listNext(l, NULL);
    e = listNext(l, e);
    fprintf(stderr, "char: %c\n", *((char*)(e->p)));
    listDelete(l);
}




void main(int argc, char* argv[]) {
    testAdd();
    testNext();
}