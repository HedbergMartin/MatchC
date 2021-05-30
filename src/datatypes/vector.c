#include "vector.h"
//#include "time.h"
#include <memory.h>
#include <stdio.h>
#include <errno.h>

/*
 * Implementation of C++ like vector in C.
 *
 * Author: Martin Hedberg
 *
 * Version information:
 *   2021-05-13: v1.1, added back poping.
 *   2021-04-21: v1.0, first implementation.
 */

struct vector {
	void** data;
	size_t capacity;
	size_t default_capacity;
	size_t size;
};

vector *vector_init() {
	vector* v = calloc(1, sizeof(vector));
	if (!v) {
		perror("Vector");
		exit(1);
	}

	v->default_capacity = 10;
	return v;
}

void vector_reserve(vector* v, size_t capacity) {
	if (!v) {
		return; //Error
	}

	v->default_capacity = capacity;
}

/*double timePushBack = 0;
clock_t pushBackStart ;
clock_t pushBackEnd;*/

void vector_push_back(vector* v, void* value) {
	//pushBackStart = clock();

	if (!v) {
		return; //Error?
	}

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
	/*pushBackEnd = clock();
	timePushBack += (double)(pushBackEnd - pushBackStart) / CLOCKS_PER_SEC;*/
}

/*double timePopBack = 0;
clock_t popBackStart;
clock_t popBackEnd;*/

void vector_pop_back(vector* v, free_func_callback free_func) {
	//popBackStart = clock();

	if (!v) {
		return;
	}

	if (v->size > 0) {
		v->size--;
		if (free_func) {
			free_func(v->data[v->size]);
		}
	}
	/*popBackEnd = clock();
	timePopBack += (double)(popBackEnd - popBackStart) / CLOCKS_PER_SEC;*/
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

void vector_clear(vector* v, free_func_callback free_func) {
	if (free_func) {
		for (int i = 0; i < v->size; i++) {
			free_func(v->data[i]);
		}
	}
	v->size = 0;
}

/*void vector_print_push_pop() {
	fprintf(stderr, "Time Pushback: %f, Time popback: %f\n", timePushBack, timePopBack);
}*/

void vector_free(vector* v, free_func_callback free_func) {
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