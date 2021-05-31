#include "branch_vector.h"
//#include "time.h"
#include <memory.h>
#include <stdio.h>
#include <errno.h>

struct branch_vector {
	net_branch* data;
	size_t capacity;
	size_t default_capacity;
	size_t size;
};

branch_vector *branch_vector_init() {
	branch_vector* v = calloc(1, sizeof(branch_vector));
	if (!v) {
		perror("branch_vector");
		exit(1);
	}

	v->default_capacity = 10;
	return v;
}

void branch_vector_reserve(branch_vector* v, size_t capacity) {
	v->default_capacity = capacity;
}

/*double timePushBack = 0;
clock_t pushBackStart ;
clock_t pushBackEnd;*/

void branch_vector_push_back(branch_vector* v, void* value) {
	if (v->capacity <= v->size) {
		if (v->capacity == 0) {
			v->capacity = v->default_capacity;
			v->data = malloc(v->capacity * sizeof(void*));
		} else {
			void** newDataLoc = malloc(v->capacity * 3 * sizeof(void*));
			memcpy(newDataLoc, v->data, v->capacity * sizeof(void*));
			free(v->data);
			v->data = newDataLoc;
			v->capacity *= 2;
		}
	}

	v->data[v->size] = value;
	v->size++;
}

void branch_vector_pop_back(branch_vector* v, free_func_callback free_func) {

	if (v->size > 0) {
		v->size--;
		if (free_func) {
			free_func(v->data[v->size]);
		}
	}
}

net_branch* branch_vector_at(branch_vector* v, size_t index) {
	return v->data[index];
}

void branch_vector_set(branch_vector* v, size_t index, void* value) {
	v->data[index] = value;
}

size_t branch_vector_size(branch_vector* v) {
	return v->size;
}

size_t branch_vector_capacity(branch_vector* v) {
	return v->capacity;
}

void branch_vector_clear(branch_vector* v, free_func_callback free_func) {
	if (free_func) {
		for (int i = 0; i < v->size; i++) {
			free_func(v->data[i]);
		}
	}
	v->size = 0;
}

/*void branch_vector_print_push_pop() {
	fprintf(stderr, "Time Pushback: %f, Time popback: %f\n", timePushBack, timePopBack);
}*/

void branch_vector_free(branch_vector* v, free_func_callback free_func) {
	if (!v) {
		return; //Error
	}

	if (free_func) {
		for (int i = 0; i < v->size; i++) {
			free_func(v->data[i]);
		}
	}

	if (v->data != NULL) {
		free(v->data);
	}
	free(v);
	
}