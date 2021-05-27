#include "int_vector.h"
#include <memory.h>
#include <stdio.h>
#include <errno.h>

struct int_vector {
	int* data;
	size_t capacity;
	size_t default_capacity;
	size_t size;
};

int_vector *int_vector_init() {
	int_vector* v = calloc(1, sizeof(int_vector));
	if (!v) {
		perror("Vector");
		exit(1);
	}

	v->default_capacity = 10;
	return v;
}

void int_vector_reserve(int_vector* v, size_t capacity) {
	if (!v) {
		return; //Error
	}

	v->default_capacity = capacity;
}

void int_vector_push_back(int_vector* v, int value) {
	if (!v) {
		return; //Error?
	}

	if (v->capacity <= v->size) {
		if (v->capacity == 0) {
			v->capacity = v->default_capacity;
			v->data = malloc(v->capacity * sizeof(int));
		} else {
			int* newDataLoc = malloc(v->capacity * 2 * sizeof(int));
			memcpy(newDataLoc, v->data, v->capacity * sizeof(int));
			free(v->data);
			v->data = newDataLoc;
			v->capacity *= 2;
		}
	}

	v->data[v->size] = value;
	v->size++;
}

void int_vector_pop_back(int_vector* v) {
	if (!v) {
		return;
	}

	if (v->size > 0) {
		v->size--;
	}
}

int int_vector_at(int_vector* v, size_t index) {
	if (!v) {
		return 0; //Error?
	}

	if (v->size <= index) {
		return 0; //Out of bounds
	}

	return v->data[index];
}

void int_vector_set(int_vector* v, size_t index, int value) {
	if (!v) {
		return; //Error?
	}

	if (v->size <= index) {
		return; //Out of bounds
	}

	v->data[index] = value;
}

size_t int_vector_size(int_vector* v) {
	if (!v) {
		return 0; //Error
	}

	return v->size;
}

size_t int_vector_capacity(int_vector* v) {
	if (!v) {
		return 0; //Error
	}

	return v->capacity;
}

void int_vector_clear(int_vector* v) {
    if (!v) {
        return;
    }
	v->size = 0;
}

void int_vector_free(int_vector* v) {
	if (!v) {
		return; //Error
	}

	if (v->data != NULL) {
		free(v->data);
	}
	free(v);
}