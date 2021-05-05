#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "queue.h"

#define CACHELINE_SIZE 64
#define HOLDER_SIZE 7 //((sizeof(queueElemHolder*) - CACHELINE_SIZE) / sizeof(void*))

typedef struct queueElemHolder {
	struct queueElemHolder* next;
	void* val[HOLDER_SIZE];
} queueElemHolder;

typedef struct queue {
	queueElemHolder* first;
	int firstI;
	queueElemHolder* end;
	int endI;
	void (*free_func)(void*);
} queue;

static void new_elemHolder(queue *q) {
	//int numOfElems = (sizeof(queueElemHolder*) - CACHELINE_SIZE) / sizeof(void*);
	//queueElemHolder* qe = calloc(1, sizeof(queueElemHolder*) + HOLDER_SIZE * sizeof(void*));
	queueElemHolder* qe = calloc(1, sizeof(queueElemHolder));

	if (q->first == NULL) {
		q->first = qe;
	} else {
		q->end->next = qe;
	}
	q->end = qe;
	q->endI = 0;

	//fprintf(stderr, "NEWLY CREATED!!!\n");
}

queue *queue_empty(void (*free_func)(void*)) {
	queue *q = calloc(1, sizeof(queue));
	q->free_func = free_func;
	new_elemHolder(q);
	return q;
}

bool queue_isEmpty(queue *q) {
	assert(q != NULL);

	if (q->first == NULL || q->first == q->end && q->endI == 0) {
		return true;
	}
	return false;
}

int enqueue(queue *q, void *val) {
	assert(q != NULL);

	if (q->endI == HOLDER_SIZE) {
		new_elemHolder(q);
	}

	q->end->val[q->endI] = val;
	q->endI += 1;

	return 0;
}

/*void* queue_inspect(queue *q) {
	assert(q != NULL);

	return list_inspect(q->l, q->first);
}*/

//Doesn't free the element after dequeue
void* dequeue(queue *q) {
	assert(q != NULL);

	if (queue_isEmpty(q)) {
		return NULL;
	}

	void* val = q->first->val[q->firstI];
	q->firstI += 1;
	
	if (q->firstI == HOLDER_SIZE) {
		queueElemHolder* qeTemp = q->first;
		q->first = q->first->next;
		free(qeTemp);
		q->firstI = 0;
	}
	

	return val;
}

void queue_kill(queue *q) {
	assert(q != NULL);

	bool first = true;

	while (q->first != NULL) {
		queueElemHolder* qeTemp = q->first;
		q->first = q->first->next;

		if (q->free_func != NULL) {

			int start = 0;

			if (first) {
				start = q->firstI;
			}

			for (size_t i = start; i < HOLDER_SIZE; i++) {

				if (qeTemp->val[i] != NULL) {
					q->free_func(qeTemp->val[i]);
				}	
			}
		}	
		first = false;	
		free(qeTemp);
	}
	free(q);
}