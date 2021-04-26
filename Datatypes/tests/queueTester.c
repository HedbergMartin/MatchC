#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include "../queue.h"

void free_func(void* val) {

    free((int*)val);
}

void testEmpty() {
    queue* q = queue_empty(NULL);
    assert(queue_isEmpty(q) == true);

    queue_kill(q);

    fprintf(stdout, "testEmpty succeeded\n");
}

void testNonEmpty() {
    queue* q = queue_empty(free_func);
    int* a = malloc(sizeof(int));
    enqueue(q, a);

    assert(queue_isEmpty(q) == false);
    queue_kill(q);

    fprintf(stdout, "testNonEmpty succeeded\n");
}

void testNonEmptyCacheLine() {
    queue* q = queue_empty(free_func);

    for (size_t i = 0; i < 8; i++) {
        int* a = malloc(sizeof(int));
        enqueue(q, a);
    }
    
    assert(queue_isEmpty(q) == false);
    queue_kill(q);

    fprintf(stdout, "testNonEmptyCacheLine succeeded\n");
}

void testEnqueueDequeue() {
    queue* q = queue_empty(free_func);
    int* a = malloc(sizeof(int));
    *a = 5;
    enqueue(q, a);

    int* b = dequeue(q);

    assert(a == b);
    free(b);
    queue_kill(q);

    fprintf(stdout, "testEnqueueDequeue succeeded\n");
}


void testEnqueueDequeueMany() {
    queue* q = queue_empty(free_func);

    for (int i = 0; i < 100; i++) {
        int* a = malloc(sizeof(int));
        *a = i;
        enqueue(q, a);
    }
    bool same = true;

    for (int i = 0; i < 100; i++) {   
        int* a = dequeue(q);

        if (*a != i) {
            fprintf(stderr, "%d, %d\n", *a, i);
            same = false;
            break;
        }
        free(a);
    }
    
    assert(same);
    queue_kill(q);

    fprintf(stdout, "testEnqueueDequeueMany succeeded\n");
}


void testOverDequeue() {

    queue* q = queue_empty(free_func);

    for (int i = 0; i < 10; i++) {
        int* a = malloc(sizeof(int));
        *a = i;
        enqueue(q, a);
    }

    for (int i = 0; i < 11; i++) {   
        int* a = dequeue(q);

        if (a != NULL) {
            free(a);
        }

    }

    queue_kill(q);

    fprintf(stdout, "testOverDequeue succeeded\n");
}

void testShitloads() {

    queue* q = queue_empty(free_func);
    int* a = malloc(sizeof(int));

    for (int i = 0; i < 1000000; i++) {
        enqueue(q, a);
    }

    for (int i = 0; i < 1000000; i++) {   
        int* a = dequeue(q);
    }

    free(a);
    queue_kill(q);

    //fprintf(stdout, "testOverDequeue succeeded\n");
}


void main(int argc, void* argv[]) {
    testEmpty();
    testNonEmpty();
    testNonEmptyCacheLine();
    testEnqueueDequeue();
    testEnqueueDequeueMany();
    testOverDequeue();

    clock_t begin = clock();
    testShitloads();
    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

    fprintf(stdout, "timespent: %f\n", time_spent);
}