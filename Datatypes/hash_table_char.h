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
    //Would need a hash_func in and a free func
};

struct hash_table* create_hash_table(int hash_size);
int insert_entry(struct hash_table* ht, char* key, void* val, size_t key_size);
//struct hash_entry* find_entry(struct hash_table ht, char* key, size_t key_size);
void* get_entry(struct hash_table* ht, char* key, size_t key_size);
void delete_hash_table(struct hash_table* ht);

#endif