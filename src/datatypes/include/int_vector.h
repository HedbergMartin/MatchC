#ifndef MC_INT_VECTOR
#define MC_INT_VECTOR

#include <stdlib.h>

typedef struct int_vector int_vector;

// Creates a empty int_int_vector.
int_vector *int_vector_init();

// Sets the default size of the int_vector when first allocation occurs.
void int_vector_reserve(int_vector* v, size_t capacity);

// Pushes an element to the end of the int_vector. Expands the list
void int_vector_push_back(int_vector* v, int value);

// Pops the last element in the int_vector
void int_vector_pop_back(int_vector* v);

// Inspect an element
int int_vector_at(int_vector* v, size_t index);

// Replace an element
void int_vector_set(int_vector* v, size_t index, int value);

// Gets the size of the int_vector
size_t int_vector_size(int_vector* v);

// Gets the capacity alloctated
size_t int_vector_capacity(int_vector* v);

void int_vector_clear(int_vector* v);

// Frees the int_vector, free_func for each entry
void int_vector_free(int_vector* v);

#endif