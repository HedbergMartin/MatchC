#include <stdlib.h>

#ifndef HASH_TBL_CHAR
#define HASH_TBL_CHAR

struct hash_entry {
    char* key;
    void* val;
    //struct hash_entry* next;
};

struct hash_table {
    struct hash_entry* entries;
    int size;
    void (*free_func)(void*);
    //Would need a hash_func in and a free func
};

struct hash_table* hash_table_create(int hash_size, void (*free_func)(void*));
int hash_table_insert(struct hash_table* ht, char* key, void* val, size_t key_size);
//struct hash_entry* find_entry(struct hash_table ht, char* key, size_t key_size);
void* hash_table_get(struct hash_table* ht, char* key, size_t key_size);
void hash_table_delete(struct hash_table* ht);

#endif