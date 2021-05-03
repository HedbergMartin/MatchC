#include "vector.h"
#include <memory.h>
#include <stdio.h>
#include <errno.h>

/*
 * Implementation of C++ like vector in C.
 *
 * Author: Martin Hedberg (96mah002@gmail.com)
 *
 * Version information:
 *   2021-04-21: v1.0, first implementation.
 */

struct vector {
	void** data;
	size_t capacity;
	size_t default_capacity;
	size_t size;
};

vector *vector_init() {
	vector* v = malloc(sizeof(vector));
	if (!v) {
		perror("Vector");
		exit(1);
	}

	v->capacity = 0;
	v->default_capacity = 10;
	v->size = 0;
}

void vector_reserve(vector* v, size_t capacity) {
	if (!v) {
		return; //Error
	}

	v->default_capacity = capacity;
}

void* vector_push_back(vector* v, void* value) {
	if (!v) {
		return NULL; //Error?
	}

	if (v->capacity <= v->size) {
		if (v->capacity == 0) {
			v->capacity = v->default_capacity;
			v->data = malloc(v->capacity * sizeof(void*));
		} else {
			void** newDataLoc = malloc(v->capacity * 2 * sizeof(void*));
			memcpy(newDataLoc, v->data, v->capacity * sizeof(void*));
			free(v->data);
			v->data = newDataLoc;
			v->capacity *= 2;
		}
	}

	v->data[v->size] = value;
	v->size++;
}

void* vector_at(vector* v, size_t index) {
	if (!v) {
		return NULL; //Error?
	}

	if (v->size <= index) {
		return NULL; //Out of bounds
	}

	return v->data[index];
}

void** vector_data(vector* v) {
	if (!v) {
		return NULL; //Error?
	}

	return v->data;
}

void vector_set(vector* v, size_t index, void* value) {
	if (!v) {
		return; //Error?
	}

	if (v->size <= index) {
		return; //Out of bounds
	}

	v->data[index] = value;
}

size_t vector_size(vector* v) {
	if (!v) {
		return 0; //Error
	}

	return v->size;
}

size_t vector_capacity(vector* v) {
	if (!v) {
		return 0; //Error
	}

	return v->capacity;
}

void vector_free(vector* v, free_func_callback free_func) {
	if (!v) {
		return; //Error
	}

	if (free_func) {
		for (int i = 0; i < v->size; i++) {
			free_func(v->data[i]);
		}
	}

	free(v->data);
	free(v);
}