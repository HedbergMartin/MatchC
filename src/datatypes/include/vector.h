#ifndef MC_VECTOR
#define MC_VECTOR

#include <stdlib.h>

/*
 * A implementation of a C++ vector in C.
 *
 * Author: Martin Hedberg 
 *
 * Version information:
 *   2021-05-13: v1.1, added back poping.
 *   2021-04-21: v1.0, first implementation.
 */

typedef struct vector vector;
typedef void (*free_func_callback)(void *);

// Creates a empty vector.
vector *vector_init();

// Sets the default size of the vector when first allocation occurs.
void vector_reserve(vector* v, size_t capacity);

// Pushes an element to the end of the vector. Expands the list
void vector_push_back(vector* v, void* value);

// Pops the last element in the vector
void vector_pop_back(vector* v, free_func_callback free_func);

// Inspect an element
void* vector_at(vector* v, size_t index);

void** vector_data(vector* v);

// Replace an element
void vector_set(vector* v, size_t index, void* value);

// Gets the size of the vector
size_t vector_size(vector* v);

// Gets the capacity alloctated
size_t vector_capacity(vector* v);

void vector_clear(vector* v, free_func_callback free_func);

void** vector_copy(vector* v);

// Frees the vector, free_func for each entry
void vector_free(vector* v, free_func_callback free_func);

//void vector_print_push_pop();

#endif