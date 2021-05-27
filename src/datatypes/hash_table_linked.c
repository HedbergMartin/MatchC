#include "hash_table_linked.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/**
 * Hash-function is bad!
 **/
static size_t hash_func(char* key, int hash_size) {
   int hash_val = 0;
   int i = 0;

   while (key[i] != '\0') {
      hash_val += key[i];
      i += 1;
   }
   return (hash_val % hash_size);
}

/**
 * 
 **/
struct hash_table* create_hash_table(int hash_size) {
    struct hash_table* ht = malloc(sizeof(struct hash_table));
    ht->entries = calloc(hash_size, sizeof(struct hash_entry**));
    ht->size = hash_size;    
    return ht;
}

struct hash_entry* create_hash_entry(char* key, int id ) {
    struct hash_entry* hte = calloc(1, sizeof(struct hash_entry));

    hte->key = key;
    hte->id = id;

    return hte;
}

struct hash_entry* find_entry(struct hash_table* ht, char* key) {
    size_t index = hash_func(key, ht->size);
    struct hash_entry* hte_temp = ht->entries[index];

    if (hte_temp != NULL) {
        
        while (hte_temp != NULL) {

            if (strcmp(hte_temp->key, key) == 0) {
                break;
            }
            hte_temp = hte_temp->next;
        }
    }
    return hte_temp;
}
        
int insert_if_absent(struct hash_table* ht, char* key, int* nextId ) {
    size_t index = hash_func(key, ht->size);

    struct hash_entry* hte_temp = find_entry(ht, key);

    if (hte_temp != NULL) {
        return hte_temp->id;
    } else {
        struct hash_entry* hte = create_hash_entry(key, *nextId);
        *nextId += 1;
        hte->next = ht->entries[index];
        ht->entries[index] = hte;

        if (hte->next != NULL) {
            hte->next->prev = hte;
        }
        return hte->id;
    }
}

/**
 * Assumes entry is known to not exist or OK with overwrite as it will never be reached again.
 **/
void insert_entry_unsafe(struct hash_table* ht, char* key, int id ) {
    size_t index = hash_func(key, ht->size);

    struct hash_entry* hte = create_hash_entry(key, id);
    hte->next = ht->entries[index];
    ht->entries[index] = hte;

    if (hte->next != NULL) {
        hte->next->prev = hte;
    }
}

void insert_entry(struct hash_table* ht, char* key, int id ) {
    size_t index = hash_func(key, ht->size);

    struct hash_entry* hte_temp = find_entry(ht, key);

    if (hte_temp != NULL) {
        fprintf(stderr, "Overwritting id (shouldn't happen)\n");
        hte_temp->id = id;
    } else {
        struct hash_entry* hte = create_hash_entry(key, id);
        hte->next = ht->entries[index];
        ht->entries[index] = hte;

        if (hte->next != NULL) {
            hte->next->prev = hte;
        }
    }
}

int get_entry(struct hash_table* ht, char* key) {
    size_t index = hash_func(key, ht->size);

    struct hash_entry* hte_temp = find_entry(ht, key);

    if (hte_temp == NULL) {
        return -1;
    } else {
        return hte_temp->id;
    }
}


void remove_entry(struct hash_table* ht, char* key ) {
    size_t index = hash_func(key, ht->size);

    struct hash_entry* hte_temp = find_entry(ht, key);

    if (hte_temp != NULL) {
        
        if (ht->entries[index] == hte_temp) {       //Is first
            
            if (hte_temp->next == NULL) {   //Is last
                ht->entries[index] = NULL;
            } else {                        //Has next
                ht->entries[index] = hte_temp->next;
                hte_temp->next->prev = NULL;
            }
        } else {                            //Has previous

            if (hte_temp->next == NULL) {   //Is last
                hte_temp->prev->next = NULL;
            } else {                        //Has next
                hte_temp->prev->next = hte_temp->next;
                hte_temp->next->prev = hte_temp->prev;
            }
        }

        free(hte_temp);
    }
}


static void delete_hash_chain(struct hash_table* ht, int index) {
    struct hash_entry* hte_next = ht->entries[index];

    while (hte_next != NULL) {
        struct hash_entry* hte_temp = hte_next;
        hte_next = hte_temp->next;

        free(hte_temp);
    }
    
}

void delete_hash_table(struct hash_table* ht) {

    for (int i = 0; i < ht->size; i++) {
        delete_hash_chain(ht, i);
    }

    free(ht->entries);
    free(ht);
}