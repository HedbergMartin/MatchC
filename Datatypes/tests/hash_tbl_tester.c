#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include "../hash_table_char.h"


void free_func(void* val) {

    free((int*)val);
}


void test_insert() {
    struct hash_table* ht = hash_table_create(10, free_func);
    char key[] = {'1', '\0'};
    int* val = malloc(sizeof(int));

    hash_table_insert(ht, key, val, 1);
    int* ret = NULL;
    ret = hash_table_get(ht, key, 1);

    assert(ret != NULL && *val == *ret);
    fprintf(stdout, "test_insert successful\n");

    hash_table_delete(ht);
}

void test_already_inserted() {
    struct hash_table* ht = hash_table_create(10, free_func);
    char key[] = {'1', '\0'};
    int* val = malloc(sizeof(int));

    hash_table_insert(ht, key, val, 1);
    int ret = hash_table_insert(ht, key, val, 1);

    assert(ret == -1);
    fprintf(stdout, "test_already_inserted successful\n");

    hash_table_delete(ht);
}

void test_overfill() {
    struct hash_table* ht = hash_table_create(1, free_func);
    char key[] = {'1', '\0'};
    char key2[] = {'2', '\0'};
    int* val = malloc(sizeof(int));

    hash_table_insert(ht, key, val, 1);
    int ret = hash_table_insert(ht, key2, val, 1);

    assert(ret == -1);
    fprintf(stdout, "test_overfill successful\n");

    hash_table_delete(ht);
}

void test_alot_fill() {
    int count = 1100;
    struct hash_table* ht = hash_table_create(count, free_func);
    bool failed = false;

    for (int i = 0; i < count; i++) {
        char* key = calloc(5, 1);
        sprintf(key, "%d", i);
        int* val = malloc(sizeof(int));
        int ret = hash_table_insert(ht, key, val, strlen(key));
        
        int* valRet = hash_table_get(ht, key, strlen(key));

        if (ret == -1 || val != valRet) {
            fprintf(stderr, "failed on: %d, w. key: %s, and %d and %d\n", i, key, ret, valRet != val);
            failed = true;
            break;
        }
        free(key);
    }
    assert(!failed);
    
    fprintf(stdout, "test_alot_fill successful\n");
    hash_table_delete(ht);
}
/*test_*/

void main(int argc, char* argv[]) {
    test_insert();
    test_already_inserted();
    test_overfill();
    test_alot_fill();
}