#include "hash_table_int.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static size_t hash_func(int key, int hash_size) {
   return (key % hash_size);
}

struct hash_table* create_hash_table(int hash_size) {
    struct hash_table* ht = calloc(1, sizeof(struct hash_table) + hash_size * sizeof(struct hash_entry)); //Weird allocation but cache-hit efficient
    ht->entries = ht + 1;
    ht->size = hash_size;    
    return ht;
}

static void put_hash_entry(int key, void* val, struct hash_entry hte) {

    hte.key = key;
    hte.val = val;
}

static struct hash_entry find_entry(struct hash_table* ht, int key, size_t hash_val) {
    struct hash_entry hte_temp = ht->entries[hash_val];
    size_t i = 0;

    if (hte_temp.val != NULL) {
        
        while (hte_temp.val != NULL && i < ht->size) {

            if (key == hte_temp.key) {
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
* @return -1 on already occupied hash key, -2 on NULL val
 **/
int insert_entry(struct hash_table* ht, int key, void* val) {
    size_t hash_val = hash_func(key, ht->size);

    struct hash_entry hte_temp = find_entry(ht, key, hash_val);

    if (hte_temp.key != NULL) {
        return -1; //Hash key already occupied
    } else if (val == NULL) {
        return -2;
    } else {
        put_hash_entry(key, val, hte_temp);
    }
}


/**
 * 
 * 
 * @return -1 on invalid key
 **/
void* get_entry(struct hash_table* ht, int key) {
    size_t hash_val = hash_func(key, ht->size);
    struct hash_entry hte_temp = find_entry(ht, key, hash_val);

    return hte_temp.val;
}


void delete_hash_table(struct hash_table* ht) {

    for (int i = 0; i < ht->size; i++) {
        
        if (ht->entries[i].key != NULL) {
            //!Free using val free func.
        }
    }

    free(ht->entries);
    free(ht);
}