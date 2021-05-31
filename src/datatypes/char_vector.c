#include "char_vector.h"
#include <memory.h>
#include <stdio.h>
#include <errno.h>

struct char_vector {
	char* data;
	size_t capacity;
	size_t default_capacity;
	size_t size;
};

char_vector *char_vector_init() {
	char_vector* v = calloc(1, sizeof(char_vector));
	if (!v) {
		perror("Vector");
		exit(1);
	}

	v->default_capacity = 10;
	return v;
}

void char_vector_reserve(char_vector* v, size_t capacity) {
	v->default_capacity = capacity;
}

void char_vector_push_back(char_vector* v, char value) {
	if (v->capacity <= v->size) {
		if (v->capacity == 0) {
			v->capacity = v->default_capacity;
			v->data = malloc(v->capacity * sizeof(char));
		} else {
			char* newDataLoc = malloc(v->capacity * 2 * sizeof(char));
			memcpy(newDataLoc, v->data, v->capacity * sizeof(char));
			free(v->data);
			v->data = newDataLoc;
			v->capacity *= 2;
		}
	}

	v->data[v->size] = value;
	v->size++;
}

void char_vector_pop_back(char_vector* v) {
	if (v->size > 0) {
		v->size--;
	}
}

char char_vector_at(char_vector* v, size_t index) {
	// if (v->size <= index) {
	// 	return 0; //Out of bounds
	// }

	return v->data[index];
}

void char_vector_set(char_vector* v, size_t index, char value) {
	// if (v->size <= index) {
	// 	return; //Out of bounds
	// }

	v->data[index] = value;
}

size_t char_vector_size(char_vector* v) {
	return v->size;
}

size_t char_vector_capacity(char_vector* v) {
	return v->capacity;
}

void char_vector_clear(char_vector* v) {
    v->size = 0;
}

char* char_vector_copy(char_vector* v) {
	if (!v) {
		return NULL;
	}

	if (v->size == 0) {
		return NULL;
	}

	char* copy = malloc(v->size * sizeof(char));
	memcpy(copy, v->data, v->size);
	return copy;
}

void char_vector_free(char_vector* v) {
	if (v->data != NULL) {
		free(v->data);
	}
	free(v);
}