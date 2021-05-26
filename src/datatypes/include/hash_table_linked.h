#include <stdlib.h>

#ifndef HASH_TBL_LINKD
#define HASH_TBL_LINKD

typedef struct hash_entry {
    char* key;
    int id;
    struct hash_entry* next;
    struct hash_entry* prev;
}hash_entry;

typedef struct hash_table {
    struct hash_entry** entries;
    int size;
}hash_table;

struct hash_table* create_hash_table(int hash_size);
int insert_if_absent(struct hash_table* ht, char* key, int* nextId);
void insert_entry_unsafe(struct hash_table* ht, char* key, int id);
void insert_entry(struct hash_table* ht, char* key, int id);
void remove_entry(struct hash_table* ht, char* key);
void delete_hash_table(struct hash_table* ht);

#endif