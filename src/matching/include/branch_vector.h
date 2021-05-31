#ifndef MC_VECTOR
#define MC_VECTOR

#include <stdlib.h>

//Can use bitsetting, but seems unneccessary with this few options
#define M_SYMBOL 1
#define M_VARIABLE 2
#define M_SEQUENCE 3
#define M_STAR 4
#define M_FUNCTION_SYMBOL 5
#define M_FUNCTION_VARIABLE 6

typedef struct branch_vector branch_vector;

typedef struct net_branch {
    char* pattern;
    int id;
	char match_settings;
    branch_vector* subnets;
} net_branch;

// Creates a empty branch_vector.
branch_vector *branch_vector_init();

// Sets the default size of the branch_vector when first allocation occurs.
void branch_vector_reserve(branch_vector* v, size_t capacity);

// Pushes an element to the end of the branch_vector. Expands the list
void branch_vector_push_back(branch_vector* v, int id, char match_settings);

// Inspect an element
net_branch* branch_vector_at(branch_vector* v, size_t index);

// Gets the size of the branch_vector
size_t branch_vector_size(branch_vector* v);

// Gets the capacity alloctated
size_t branch_vector_capacity(branch_vector* v);

// Frees the branch_vector, free_func for each entry
void branch_vector_free(branch_vector* v);

//void branch_vector_print_push_pop();

#endif