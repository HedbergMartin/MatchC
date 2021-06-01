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
    int nextId;
}hash_table;

struct hash_table* hash_table_init(int hash_size, int startId);
int hash_table_insert_if_absent(struct hash_table* ht, char* key);
void hash_table_insert_entry_unsafe(struct hash_table* ht, char* key, int id);
void hash_table_insert_entry(struct hash_table* ht, char* key, int id);
int hash_table_get_entry(struct hash_table* ht, char* key);
int hash_table_nextId(struct hash_table* ht);
void hash_table_remove_entry(struct hash_table* ht, char* key);
void hash_table_free(struct hash_table* ht);

#endif