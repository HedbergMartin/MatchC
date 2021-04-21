#include <stdlib.h>

#ifndef HASH_TBL
#define HASH_TBL

struct hash_entry {
    char* key;
    char* msg;
    size_t msg_size;
    struct hash_entry* next;
    struct hash_entry* prev; //Can we be bothered to remove prev? Not really necessary (realized to late...)
};

struct msg_ret {
    char* msg;
    size_t msg_size;
};

struct hash_table {
    struct hash_entry** entries;
    int size;
};

struct hash_table create_hash_table(int hash_size);
void insert_entry(struct hash_table ht, char* key, char* msg, size_t msg_size, size_t key_size);
struct hash_entry* find_entry(struct hash_table ht, char* key, size_t key_size);
void remove_entry(struct hash_table ht, char* key, size_t key_size);
struct msg_ret get_entry(struct hash_table ht, char* key, size_t key_size);
void delete_hash_table(struct hash_table ht);

#endif