#ifndef MC_S_vector
#define MC_S_vector

#include <stdlib.h>
#include "flatterm.h"
#include "subjectFlatterm.h"

//TODO this is just a int vector now
typedef struct s_vector s_vector;
typedef struct s_entry {
    int from;
} s_entry;

typedef struct sub_arr_entry {
    subjectFlatterm* to;
    int len;
} sub_arr_entry;

// Creates a empty s_vector.
s_vector* s_vector_init();

// Pushes an element to the end of the s_vector. Expands the list
void s_vector_push_back(s_vector* v, int from);

// Pops the last element in the s_vector
void s_vector_pop_back(s_vector* v);

// Inspect an element
int s_vector_at(s_vector* v, size_t index);

// Gets the size of the vector
size_t s_vector_size(s_vector* v);

s_entry* s_vector_copy(s_vector* v, int* len);

void s_vector_clear(s_vector* v);

// Frees the s_vector, free_func for each entry
void s_vector_free(s_vector* v);

#endif