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
    //Would need a hash_func in and a free func
};

struct hash_table* create_hash_table(int hash_size);
int insert_entry(struct hash_table* ht, int key, void* val);
//struct hash_entry* find_entry(struct hash_table ht, char* key, size_t key_size);
void* get_entry(struct hash_table* ht, int key);
void delete_hash_table(struct hash_table* ht);

#endif