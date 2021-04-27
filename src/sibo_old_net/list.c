#include <stdio.h>
#include <stdlib.h>
#include "list.h"


/**
 * Creates 
 */
struct list* listCreate(void (*free_elem)(void* e)) {
    struct list* l = calloc(1, sizeof(struct list));
    l->free_elem = free_elem;
    return l;
}

/**
 * 
 */
void listRemove(struct list* l, struct listElem* e) { 

    if (e->next != NULL) {
        e->next->prev = e->prev;
    } else {
        l->last = e->prev;
    }

    if (e->prev != NULL) {
        e->prev->next = e->next;
    } else {
        l->first = e->next;
    }

    if (l->free_elem != NULL) {
        l->free_elem(e->p);
    }
    free(e);

    return;
}

/**
 * Adds element to the end of the list
 */
void listAdd(struct list* l, void* val) {
    struct listElem* e = calloc(1, sizeof(struct listElem));
    e->p = val;
    e->prev = l->last;

    if (l->last != NULL) {
        l->last->next = e;
    }
    l->last = e;
    
    if (l->first == NULL) {
        l->first = e;
    }
}

/**
 * Gets the next element in the list.
 * To get the first element set @param e to NULL
 * Returns NULL if there are no more arguments
 */
struct listElem* listNext(struct list* l, struct listElem* e) {

    if (e == NULL) {
        return l->first;
    } else {
        return e->next;
    }   
}

/**
 * Frees the entire list
 */
void listDelete(struct list* l) {

    while (l->first != NULL) {
        listRemove(l, l->first);
    }
    free(l);
}