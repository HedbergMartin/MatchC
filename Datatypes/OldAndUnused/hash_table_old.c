#include "hash_table_old.h"
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

struct hash_table create_hash_table(int hash_size) {
    struct hash_table ht;
    ht.entries = calloc(hash_size, sizeof(struct kvm_hash_entry*));
    ht.size = hash_size;    
    return ht;
}

static struct hash_entry* create_hash_entry(char* key, char* msg, size_t msg_size, size_t key_size) {
    struct hash_entry* hte = calloc(1, sizeof(struct hash_entry));

    hte->key = malloc(key_size);
    hte->msg = malloc(msg_size);
    hte->msg_size = msg_size;

    strncpy(hte->key, key, key_size);
    strncpy(hte->msg, msg, msg_size);

    return hte;
}

struct hash_entry* find_entry(struct hash_table ht, char* key, size_t key_size) {
    size_t index = hash_func(key, key_size, ht.size);
    struct hash_entry* hte_temp = ht.entries[index];

    if (hte_temp != NULL) {
        
        while (hte_temp != NULL) {

            if (strncmp(hte_temp->key, key, key_size) == 0) {
                break;
            }
            hte_temp = hte_temp->next;
        }
    }
    return hte_temp;
}
        
//TODO: DOESNT HANDLE EXACTLY SAME KEY (KEY DUPLICATION)
void insert_entry(struct hash_table ht, char* key, char* msg, size_t msg_size, size_t key_size) {
    size_t index = hash_func(key, key_size, ht.size);
    //struct kvm_hash_entry* hte = kvm_create_hash_entry(key, msg, msg_size, key_size);
    //struct kvm_hash_entry* hte_next = ht.entries[index];

    struct hash_entry* hte_temp = kvm_find_entry(ht, key, key_size);

    if (hte_temp != NULL) {
        free(hte_temp->msg);
        hte_temp->msg = malloc(msg_size);
        hte_temp->msg_size = msg_size;
        strncpy(hte_temp->msg, msg, msg_size);
    } else {
        struct kvm_hash_entry* hte = kvm_create_hash_entry(key, msg, msg_size, key_size);
        hte->next = ht.entries[index];
        ht.entries[index] = hte;

        if (hte->next != NULL) {
            hte->next->prev = hte;
        }
    }

    /*if (hte_next == NULL) {
        hte->prev = NULL;
        ht.entries[index] = hte;
    } else {
        
        while (hte_next->next != NULL) {
            hte_next = hte_next->next;
        }
        
        hte->prev = hte_next;
        hte_next->next = hte;
    }*/
}


void remove_entry(struct hash_table ht, char* key, size_t key_size) {
    size_t index = hash_func(key, key_size, ht.size);

    struct hash_entry* hte_temp = kvm_find_entry(ht, key, key_size);

    if (hte_temp != NULL) {
        
        if (hte_temp->prev == NULL) {       //Is first
            
            if (hte_temp->next == NULL) {   //Is last
                ht.entries[index] = NULL;
            } else {                        //Has next
                ht.entries[index] = hte_temp->next;
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

        free(hte_temp->msg);
        free(hte_temp->key);
        free(hte_temp);
    }
}

struct msg_ret get_entry(struct hash_table ht, char* key, size_t key_size) {
    struct hash_entry* hte_temp = kvm_find_entry(ht, key, key_size);
    struct msg_ret ret;

    if (hte_temp == NULL) {
        ret.msg = NULL;
        ret.msg_size = 0;
    } else {
        ret.msg = hte_temp->msg;
        ret.msg_size = hte_temp->msg_size;
    }

    return ret;
}

static void delete_hash_chain(struct hash_table ht, int index) {
    struct hash_entry* hte_next = ht.entries[index];

    while (hte_next != NULL) {
        struct hash_entry* hte_temp = hte_next;
        hte_next = hte_temp->next;

        free(hte_temp->msg);
        free(hte_temp->key);
        free(hte_temp);
    }
    
}

void delete_hash_table(struct hash_table ht) {

    for (int i = 0; i < ht.size; i++) {
        delete_hash_chain(ht, i);
    }

    free(ht.entries);
    
}