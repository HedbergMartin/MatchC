#ifndef MC_VARIABLE_VECTOR
#define MC_VARIABLE_VECTOR

#include <stdlib.h>

/*
 * A implementation of a C++ variable_vector in C.
 *
 * Author: Martin Hedberg 
 *
 * Version information:
 *   2021-05-13: v1.1, added back poping.
 *   2021-04-21: v1.0, first implementation.
 */

typedef struct variable_vector variable_vector;

typedef struct variable_entry {
	char* symbol;
	int type;
}variable_entry;

// Creates a empty variable_vector.
variable_vector *variable_vector_init();

// Sets the default size of the variable_vector when first allocation occurs.
void variable_vector_reserve(variable_vector* v, size_t capacity);

// Pushes an element to the end of the variable_vector. Expands the list
void variable_vector_push_back(variable_vector* v, char* symbol, int type);

// Inspect an element
variable_entry* variable_vector_at(variable_vector* v, size_t index);

// Gets the size of the variable_vector
size_t variable_vector_size(variable_vector* v);

variable_entry* variable_vector_copy(variable_vector* v);

// Frees the variable_vector, free_func for each entry
void variable_vector_free(variable_vector* v);

//void variable_vector_print_push_pop();

#endif