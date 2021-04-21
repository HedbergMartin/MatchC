#include "hash_table_char.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static size_t hash_func(char* key, size_t key_size, int hash_size) {
   int hash_val = 0;

   for (size_t i = 0; i < key_size; i++) {
      hash_val += key[i];
   }
   return (hash_val % hash_size);
}

struct hash_table* create_hash_table(int hash_size) {
    struct hash_table* ht = calloc(1, sizeof(struct hash_table) + hash_size * sizeof(struct hash_entry)); //Weird allocation but cache-hit efficient
    ht->entries = ht + 1;
    ht->size = hash_size;    
    return ht;
}

static void put_hash_entry(char* key, void* val, size_t key_size, struct hash_entry hte) {

    if (key[key_size - 1] != '\0') {
        key_size += 1;
    }

    hte.key = malloc(key_size);

    strncpy(hte.key, key, key_size);
    hte.key[key_size - 1] = '\0';
    hte.val = val;
}

static struct hash_entry find_entry(struct hash_table* ht, char* key, size_t key_size, size_t hash_val) {
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
    return hte_temp;
}

/**
 * 
 * @return -1 on already occupied hash key
 **/
int insert_entry(struct hash_table* ht, char* key, void* val, size_t key_size) {
    size_t hash_val = hash_func(key, key_size, ht->size);

    struct hash_entry hte_temp = find_entry(ht, key, key_size, hash_val);

    if (hte_temp.key != NULL) {
        return -1; //Hash key already occupied
    } else {
        put_hash_entry(key, val, key_size, hte_temp);
    }
}


/**
 * 
 * 
 * @return NULL on invalid key
 **/
void* get_entry(struct hash_table* ht, char* key, size_t key_size) {
    size_t hash_val = hash_func(key, key_size, ht->size);
    struct hash_entry hte_temp = find_entry(ht, key, key_size, hash_val);

    return hte_temp.val;
}


void delete_hash_table(struct hash_table* ht) {

    for (int i = 0; i < ht->size; i++) {
        
        if (ht->entries[i].key != NULL) {
            free(ht->entries[i].key);
            //!Free using val free func.
        }
    }

    free(ht->entries);
    free(ht);
}