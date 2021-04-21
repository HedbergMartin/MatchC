#ifndef QUEUE_H
#define QUEUE_H

#include <stdbool.h>

typedef struct queue queue;

queue *queue_empty();

bool queue_isEmpty(queue *q);

int enqueue(queue *q, void *element);

void *queue_inspect(queue *q);

void dequeue(queue *q);

void queue_kill(queue *q);

#endif