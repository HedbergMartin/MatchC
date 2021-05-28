#include <stdlib.h>

#ifndef HASH_TBL_INT
#define HASH_TBL_INT

struct hash_entry {
    int key;
    void* val;
    //struct hash_entry* next;
};

struct hash_table {
    struct hash_entry* entries;
    int size;
    void (*free_func)(void*);
    //Would need a _hash_func in and a free func
};

struct hash_table* hash_table_init(int hash_size);
int hash_table_insert_entry(struct hash_table* ht, int key, void* val);
//struct hash_entry* _hash_table_find_entry(struct hash_table ht, char* key, size_t key_size);
void* hash_table_get_entry(struct hash_table* ht, int key);
void hash_table_free(struct hash_table* ht);

#endif