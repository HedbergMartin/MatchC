#ifndef MC_FT_VECTOR
#define MC_FT_VECTOR

#include <stdlib.h>

/*
 * A implementation of a C++ vector in C.
 *
 * Author: Martin Hedberg (96mah002@gmail.com)
 *
 * Version information:
 *   2021-04-21: v1.0, first implementation.
 */

typedef struct ft_vector ft_vector;
typedef void (*free_func_callback)(void *);

// Creates a empty list with a free function that is used to free entries.
ft_vector *ft_vector_init(free_func_callback free_func);

// Sets the default size of the ft_vector when first allocation occurs.
void ft_vector_reserve(ft_vector* v, size_t capacity);

// Pushes an element to the end of the ft_vector. Expands the list
void* ft_vector_push_back(ft_vector* v, void* value);

// Inspect an element
void* ft_vector_at(ft_vector* v, size_t index);

void** ft_vector_data(ft_vector* v);

// Replace an element
void ft_vector_set(ft_vector* v, size_t index, void* value);

// Gets the size of the ft_vector
size_t ft_vector_size(ft_vector* v);

// Gets the capacity alloctated
size_t ft_vector_capacity(ft_vector* v);

// Frees the ft_vector
void ft_vector_free(ft_vector* v);

#endif