#ifndef MC_NET_BRANCH
#define MC_NET_BRANCH

#include <stdlib.h>
#include "match_types.h"
#include "variable_vector.h"

typedef struct branch_vector branch_vector;

typedef struct branch_match {
    char* pattern;
    variable_entry* variable_names;
    int len;
}branch_match;

typedef struct net_branch {
    int id;
	enum matchtype m_type; //TODO Mergo to one match type
	enum functype f_type;
    branch_vector* subnets;
    branch_match* match_data;
} net_branch;

// Creates a empty net_branch.
net_branch* net_init_root();

// Sets the default size of the net_branch when first allocation occurs.
void net_branch_reserve(net_branch* v, size_t capacity);

// Pushes an element to the end of the net_branch. Expands the list
net_branch* net_branch_add(net_branch* v, int id, enum matchtype m_type, enum functype f_type);

// Sets the match config for the branch
void net_branch_set_match(net_branch* v, char* pattern, variable_entry* variable_names, int len);

// Inspect an element
net_branch* net_branch_sub_branch(net_branch* v, size_t index);

// Gets the size of the net_branch
size_t net_branch_size(net_branch* v);

// Gets the capacity alloctated
size_t net_branch_capacity(net_branch* v);

// Finds a exact subnet
net_branch* find_subnet(net_branch* dn, int id, enum matchtype m_type, enum functype f_type);

// Frees the net_branch, free_func for each entry
void net_root_free(net_branch* v);

#endif