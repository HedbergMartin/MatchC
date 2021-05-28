#include "hash_table_char.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static size_t _hash_func(char* key, size_t key_size, int hash_size) {
   int hash_val = 0;

   for (size_t i = 0; i < key_size; i++) {
      hash_val += key[i];
   }
   return (hash_val % hash_size);
}

struct hash_table* hash_table_create(int hash_size, void (*free_func)(void*)) {
    //struct hash_table* ht = calloc(1, sizeof(struct hash_table) + hash_size * sizeof(struct hash_entry)); //Weird allocation but cache-hit efficient
    //ht->entries = (struct hash_entry*)(ht + 1);
    struct hash_table* ht = calloc(1, sizeof(struct hash_table)); 
    ht->entries = calloc(1, hash_size * sizeof(struct hash_entry));
    ht->size = hash_size;
    ht->free_func = free_func;
    return ht;
}

static void put_hash_entry(char* key, void* val, size_t key_size, struct hash_entry* hte) {

    if (key[key_size - 1] != '\0') {
        key_size += 1;
    }

    hte->key = malloc(key_size);
    memcpy(hte->key, key, key_size);
    hte->key[key_size - 1] = '\0';

    /*if (key[key_size - 1] != '\0') {
        hte->key = malloc(key_size + 1);
        memcpy(hte->key, key, key_size);
        hte->key[key_size] = '\0';
    } else {
        hte->key = malloc(key_size + 1);
        memcpy(hte->key, key, key_size);
    }*/

    hte->val = val;
}

static struct hash_entry* _hash_table_find_entry(struct hash_table* ht, char* key, size_t key_size, size_t hash_val) {
    struct hash_entry hte_temp = ht->entries[hash_val];
    size_t i = 0;

    if (hte_temp.key != NULL) {
        
        while (hte_temp.key != NULL && i < ht->size) {

            if (strcmp(key, hte_temp.key) == 0) {
                break;
            }
            
            i += 1;
            hte_temp = ht->entries[(hash_val + i) % ht->size];
        }
    }
    return &(ht->entries[(hash_val + i) % ht->size]);   //!Clean up if used, this looks bad, use hte_temp as ptr instead
}

/**
 * 
 * @return -1 on already occupied hash key
 **/
int hash_table_insert(struct hash_table* ht, char* key, void* val, size_t key_size) {
    size_t hash_val = _hash_func(key, key_size, ht->size);
    struct hash_entry* hte_temp = _hash_table_find_entry(ht, key, key_size, hash_val);

    //fprintf(stderr, "%s\n", hte_temp.key);

    if (hte_temp->key != NULL) {
        return -1; //Hash key already occupied or entire thing full
    } else {
        //fprintf(stderr, "asdasdsad\n");
        put_hash_entry(key, val, key_size, hte_temp);
        //fprintf(stderr, "asdasdsad2\n");
    }
    return 0;
}


/**
 * 
 * 
 * @return NULL on invalid key
 **/
void* hash_table_get(struct hash_table* ht, char* key, size_t key_size) {
    size_t hash_val = _hash_func(key, key_size, ht->size);
    struct hash_entry* hte_temp = _hash_table_find_entry(ht, key, key_size, hash_val);

    if (hte_temp->key == NULL || strcmp(key, hte_temp->key) != 0) {
        return NULL;
    }

    return hte_temp->val;
}


void hash_table_delete(struct hash_table* ht) {

    if (ht->free_func != NULL) {

        for (int i = 0; i < ht->size; i++) {
        
            if (ht->entries[i].key != NULL) {
                free(ht->entries[i].key);
                ht->free_func(ht->entries[i].val);
                //!Free using val free func.
            }
        }
    }

    free(ht->entries);
    free(ht);
}