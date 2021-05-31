#include "net_branch.h"
#include <memory.h>
#include <stdio.h>
#include <errno.h>

struct branch_vector {
	net_branch* data;
	size_t capacity;
	size_t default_capacity;
	size_t size;
};

branch_vector* net_branch_init_subnets() {
	branch_vector* v = calloc(1, sizeof(net_branch));
	if (!v) {
		perror("branch_vector");
		exit(1);
	}

	v->default_capacity = 10;
	return v;
}

net_branch* net_init_root() {
	net_branch* branch = calloc(1, sizeof(net_branch));
	if (!branch) {
		perror("net_branch");
		exit(1);
	}

	branch->subnets = net_branch_init_subnets();
}

void net_branch_reserve(net_branch* v, size_t capacity) {
	v->subnets->default_capacity = capacity;
}

net_branch* net_branch_add(net_branch* b, int id, enum matchtype m_type, enum functype f_type) {
	branch_vector* v = b->subnets;
	if (v->capacity <= v->size) {
		if (v->capacity == 0) {
			v->capacity = v->default_capacity;
			v->data = malloc(v->capacity * sizeof(net_branch));
		} else {
			net_branch* newDataLoc = malloc(v->capacity * 3 * sizeof(net_branch));
			memcpy(newDataLoc, v->data, v->capacity * sizeof(net_branch));
			free(v->data);
			v->data = newDataLoc;
			v->capacity *= 2;
		}
	}

	v->data[v->size].id = id;
	v->data[v->size].m_type = m_type;
	v->data[v->size].f_type = f_type;
	v->data[v->size].subnets = net_branch_init_subnets();
	v->size++;

	return &(v->data[v->size-1]);
}

void net_branch_set_match(net_branch* v, char* pattern, char** variable_names, int len) {
	branch_match* bm = malloc(sizeof(branch_match));
	bm->pattern = pattern;
	bm->variable_names = variable_names;
	bm->len = len;

	v->match_data = bm;
}

net_branch* net_branch_sub_branch(net_branch* b, size_t index) {
	return &(b->subnets->data[index]);
}

size_t net_branch_size(net_branch* b) {
	return b->subnets->size;
}

size_t net_branch_capacity(net_branch* v) {
	return v->subnets->capacity;
}

net_branch* find_subnet(net_branch* branch, int id, enum matchtype m_type, enum functype f_type) {
    for (int i = 0; i < net_branch_size(branch); i++) {
        net_branch* sn = &(branch->subnets->data[i]);

        if (sn->id == id && sn->m_type == m_type && f_type == sn->f_type) { 
            return sn;
        }
    }

    return NULL;
}

void net_branch_free(branch_vector* v) {
	if (!v) {
		return; //Error
	}

	for (int i = 0; i < v->size; i++) {
		net_branch* subbranch = &(v->data[i]);
		branch_match* bm = subbranch->match_data;
		if (bm != NULL) {
			for (int k = 0; k < bm->len; k++) {
				free(bm->variable_names[k]);
			}
			free(bm->variable_names);
		}

		net_branch_free(subbranch->subnets);
	}
	

	if (v->data != NULL) {
		free(v->data);
	}	
}

void net_root_free(net_branch* v) {
	net_branch_free(v->subnets);
	free(v);
}