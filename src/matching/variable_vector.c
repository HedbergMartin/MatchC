#include "variable_vector.h"
//#include "time.h"
#include <memory.h>
#include <stdio.h>
#include <errno.h>

/*
 * Implementation of C++ like variable_vector in C.
 *
 * Author: Martin Hedberg
 *
 * Version information:
 *   2021-05-13: v1.1, added back poping.
 *   2021-04-21: v1.0, first implementation.
 */

struct variable_vector {
	variable_entry* data;
	size_t capacity;
	size_t default_capacity;
	size_t size;
};

variable_vector *variable_vector_init() {
	variable_vector* v = calloc(1, sizeof(variable_vector));
	if (!v) {
		perror("variable_vector");
		exit(1);
	}

	v->default_capacity = 10;
	return v;
}

void variable_vector_reserve(variable_vector* v, size_t capacity) {
	v->default_capacity = capacity;
}

void variable_vector_push_back(variable_vector* v, char* symbol, int type) {
	//pushBackStart = clock();

	if (!v) {
		return; //Error?
	}

	if (v->capacity <= v->size) {
		if (v->capacity == 0) {
			v->capacity = v->default_capacity;
			v->data = malloc(v->capacity * sizeof(variable_entry));
		} else {
			variable_entry* newDataLoc = malloc(v->capacity * 3 * sizeof(variable_entry));
			memcpy(newDataLoc, v->data, v->capacity * sizeof(variable_entry));
			free(v->data);
			v->data = newDataLoc;
			v->capacity *= 2;
		}
	}

	v->data[v->size].symbol = symbol;
	v->data[v->size].type = type;
	v->size++;
}

variable_entry* variable_vector_at(variable_vector* v, size_t index) {
	return &(v->data[index]);
}

size_t variable_vector_size(variable_vector* v) {
	return v->size;
}

variable_entry* variable_vector_copy(variable_vector* v) {
	if (!v) {
		return NULL;
	}

	if (v->size == 0) {
		return NULL;
	}

	variable_entry* copy = malloc(v->size * sizeof(variable_entry));
	memcpy(copy, v->data, v->size * sizeof(variable_entry));
	return copy;
}

void variable_vector_free(variable_vector* v) {
	if (!v) {
		return; //Error
	}

	if (v->data != NULL) {
		free(v->data);
	}
	free(v);
	
}