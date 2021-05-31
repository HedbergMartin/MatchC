#ifndef MC_CHAR_VECTOR
#define MC_CHAR_VECTOR

#include <stdlib.h>

typedef struct char_vector char_vector;

// Creates a empty char_char_vector.
char_vector *char_vector_init();

// Sets the default size of the char_vector when first allocation occurs.
void char_vector_reserve(char_vector* v, size_t capacity);

// Pushes an element to the end of the char_vector. Expands the list
void char_vector_push_back(char_vector* v, char value);

// Pops the last element in the char_vector
void char_vector_pop_back(char_vector* v);

// Inspect an element
char char_vector_at(char_vector* v, size_t index);

// Replace an element
void char_vector_set(char_vector* v, size_t index, char value);

// Gets the size of the char_vector
size_t char_vector_size(char_vector* v);

// Gets the capacity alloctated
size_t char_vector_capacity(char_vector* v);

void char_vector_clear(char_vector* v);

char* char_vector_copy(char_vector* v);

// Frees the char_vector, free_func for each entry
void char_vector_free(char_vector* v);

#endif