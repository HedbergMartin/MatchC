#include <stdlib.h>
#include <assert.h>
#include "queue.h"
#include "../list.h"

typedef struct queue {
	struct list *l;
	list_pos first;
	list_pos end;
} queue;

queue *queue_empty() {
	queue *q = malloc(sizeof(queue));
	q->l = list_empty();
	assert(q->l != NULL);
	q->first = list_first(q->l);
	q->end = list_end(q->l);

	return q;
}

bool queue_isEmpty(queue *q) {
	assert(q != NULL);

	return list_isEmpty((q->l));
}

int enqueue(queue *q, void *element) {
	assert(q != NULL);
	assert(element != NULL);

	q->end = list_insert_after(q->l, q->end, element);

	if (q->end == NULL) {
		q->end = list_end(q->l);
		return 1;
	}
	return 0;
}

void *queue_inspect(queue *q) {
	assert(q != NULL);

	return list_inspect(q->l, q->first);
}

void* dequeue(queue *q) {
	assert(q != NULL);

	q->first = list_remove(q->l, q->first, NULL);
}

void queue_kill(queue *q) {
	assert(q != NULL);

	list_kill(q->l, NULL);
	free(q);
}