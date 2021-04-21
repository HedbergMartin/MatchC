#ifndef MC_VECTOR
#define MC_VECTOR

#include <stdlib.h>

/*
 * A implementation of a C++ vector in C.
 *
 * Author: Martin Hedberg (96mah002@gmail.com)
 *
 * Version information:
 *   2021-04-21: v1.0, first implementation.
 */

typedef struct vector vector;
typedef void (*free_func_callback)(void *);

// Creates a empty list with a free function that is used to free entries.
vector *vector_init(free_func_callback free_func);

// Sets the default size of the vector when first allocation occurs.
void vector_reserve(vector* v, size_t capacity);

// Pushes an element to the end of the vector. Expands the list
void* vector_push_back(vector* v, void* value);

// Inspect an element
void* vector_at(vector* v, size_t index);

// Replace an element
void vector_set(vector* v, size_t index, void* value);

// Gets the size of the vector
size_t vector_size(vector* v);

// Gets the capacity alloctated
size_t vector_capacity(vector* v);

// Frees the vector
void vector_free(vector* v);

#endif