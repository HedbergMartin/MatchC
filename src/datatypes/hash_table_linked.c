#include "hash_table_linked.h"
#include <stdio.h>
#include <string.h>

static size_t _hash_func(char* key, int hash_size);
struct hash_entry* _hash_entry_init(char* key, int id);
void _hash_table_delete_chain(struct hash_table* ht, int index);

/**
 * A lazy hash-function.
 **/
size_t _hash_func(char* key, int hash_size) {
   int hash_val = 0;
   int i = 0;

   while (key[i] != '\0') {
      hash_val += key[i];
      i += 1;
   }
   return (hash_val % hash_size);
}

/**
 * @brief Create a hash table struct
 * 
 * @param hash_size Buckets for the hash table
 * @return New hash table struct
 */
hash_table* hash_table_init(int hash_size, int startId) {
    hash_table* ht = malloc(sizeof(struct hash_table));
    ht->entries = calloc(hash_size, sizeof(struct hash_entry**));
    ht->size = hash_size;    
    ht->nextId = startId;
    return ht;
}

/**
 * @brief Creates and initializes a hash entry struct
 * 
 * @param key key, as string
 * @param id value as int
 * @return new hash_entry
 */
hash_entry* _hash_entry_init(char* key, int id) {
    struct hash_entry* hte = calloc(1, sizeof(struct hash_entry));

    hte->key = key;
    hte->id = id;

    return hte;
}

/**
 * @brief Finds a hash_entry in the hash_table
 * 
 * @param ht hash_table to search
 * @param key key for hash_entry
 * @return The found entry or NULL if not found
 */
hash_entry* _hash_table_find_entry(struct hash_table* ht, char* key) {
    size_t index = _hash_func(key, ht->size);
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

/**
 * @brief Gets the id of a key and if it doesn't exist it inserts it
 * 
 * @param ht hash_table to search
 * @param key key for new entry
 * @param nextId id to use if entry is new
 * @return id for the key
 */
int hash_table_insert_if_absent(struct hash_table* ht, char* key) {

    if (ht->size == 0) {
        fprintf(stderr, "Ht == NULL\n");
    }
    size_t index = _hash_func(key, ht->size);

    struct hash_entry* hte_temp = _hash_table_find_entry(ht, key);

    if (hte_temp != NULL) {
        return hte_temp->id;
    } else {
        struct hash_entry* hte = _hash_entry_init(key, ht->nextId );
        ht->nextId += 1;
        hte->next = ht->entries[index];
        ht->entries[index] = hte;

        if (hte->next != NULL) {
            hte->next->prev = hte;
        }
        return hte->id;
    }
}

/**
 * @brief inserts an entry without checking for overlapping keys
 * Assumes entry is known to not exist or OK with overwrite as it will never be reached again.
 * 
 * @param ht hash_table to insert into
 * @param key key for the new entry
 * @param id id for the entry
 */
void hash_table_insert_entry_unsafe(struct hash_table* ht, char* key, int id ) {
    size_t index = _hash_func(key, ht->size);

    struct hash_entry* hte = _hash_entry_init(key, id);
    hte->next = ht->entries[index];
    ht->entries[index] = hte;

    if (hte->next != NULL) {
        hte->next->prev = hte;
    }
}

/**
 * @brief inserts an entry and checks for overlapping keys
 * 
 * @param ht 
 * @param key 
 * @param id 
 */
void hash_table_insert_entry(struct hash_table* ht, char* key, int id ) {
    size_t index = _hash_func(key, ht->size);

    struct hash_entry* hte_temp = _hash_table_find_entry(ht, key);

    if (hte_temp != NULL) {
        fprintf(stderr, "Overwritting id (shouldn't happen)\n");
        hte_temp->id = id;
    } else {
        struct hash_entry* hte = _hash_entry_init(key, id);
        hte->next = ht->entries[index];
        ht->entries[index] = hte;

        if (hte->next != NULL) {
            hte->next->prev = hte;
        }
    }
}

/**
 * @brief Gets the id of the entry with specific key
 * 
 * @param ht 
 * @param key 
 * @return The id of the entry, -1 if doesn't exist
 */
int hash_table_get_entry(struct hash_table* ht, char* key) {
    size_t index = _hash_func(key, ht->size);

    struct hash_entry* hte_temp = _hash_table_find_entry(ht, key);

    if (hte_temp == NULL) {
        return -1;
    } else {
        return hte_temp->id;
    }
}

/**
 * @brief 
 * 
 * @param ht 
 * @param key 
 */
void hash_table_remove_entry(struct hash_table* ht, char* key ) {
    size_t index = _hash_func(key, ht->size);

    struct hash_entry* hte_temp = _hash_table_find_entry(ht, key);

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

/**
 * @brief 
 * 
 * @param ht 
 * @param index 
 */
void _hash_table_delete_chain(struct hash_table* ht, int index) {
    struct hash_entry* hte_next = ht->entries[index];

    while (hte_next != NULL) {
        struct hash_entry* hte_temp = hte_next;
        hte_next = hte_temp->next;

        free(hte_temp);
    }
}

int hash_table_nextId(struct hash_table* ht) {
    return ht->nextId;
}

/**
 * @brief 
 * 
 * @param ht 
 */
void hash_table_free(struct hash_table* ht) {

    for (int i = 0; i < ht->size; i++) {
        _hash_table_delete_chain(ht, i);
    }

    free(ht->entries);
    free(ht);
}