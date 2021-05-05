#ifndef QUEUE_H
#define QUEUE_H

#include <stdbool.h>

typedef struct queue queue;

queue *queue_empty(void (*free_func)(void*));

bool queue_isEmpty(queue *q);

int enqueue(queue *q, void *val);

//void *queue_inspect(queue *q);

void* dequeue(queue *q);

void queue_kill(queue *q);

#endif