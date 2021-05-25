#include "s_vector.h"
#include <memory.h>
#include <stdio.h>
#include <errno.h>

struct s_vector {
	s_entry* data;
	size_t capacity;
	size_t size;
};

s_vector *s_vector_init() {
	s_vector* v = malloc(sizeof(s_vector));
	if (!v) {
		perror("s_vector");
		exit(1);
	}

	v->capacity = 0;
	v->size = 0;
}

void s_vector_push_back(s_vector* v, char* from, subjectFlatterm* to, int len) {
	if (!v) {
		return; //Error?
	}

	if (v->capacity <= v->size) {
		if (v->capacity == 0) {
			v->capacity = 10;
			v->data = malloc(v->capacity * sizeof(s_entry));
		} else {
			s_entry* newDataLoc = malloc(v->capacity * 2 * sizeof(s_entry));
			memcpy(newDataLoc, v->data, v->capacity * sizeof(s_entry));
			free(v->data);
			v->data = newDataLoc;
			v->capacity *= 2;
		}
	}

	v->data[v->size].from = from;
	v->data[v->size].to = to;
	v->data[v->size].len = len;
	
	v->size++;
}

void s_vector_pop_back(s_vector* v) {
	if (!v) {
		return;
	}

	if (v->size > 0) {
		v->size--;
	}
}

s_entry* s_vector_copy(s_vector* v, int* len) {
	s_entry* c = malloc(v->size * sizeof(s_entry));
	memcpy(c, v->data, v->size * sizeof(s_entry));
	*len = v->size;

	return c;
}

s_entry* s_vector_at(s_vector* v, size_t index) {
	return &(v->data[index]);
}

size_t s_vector_size(s_vector* v) {
	if (!v) {
		return 0; //Error
	}

	return v->size;
}

void s_vector_clear(s_vector* v) {
	v->size = 0;
}

void s_vector_free(s_vector* v) {
	if (!v) {
		return; //Error
	}

	free(v->data);
	free(v);
}