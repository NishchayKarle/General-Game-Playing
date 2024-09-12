#ifndef _QUEUE_H
#define _QUEUE_H

#include <stdbool.h>
#include <stdlib.h>

typedef struct Node {
    void* data;
    struct Node* next;
} Node;

typedef struct Queue {
    Node* head;
    Node* tail;
    void (*free_data)(void*);
} Queue;

Queue* create_queue();

bool is_empty(Queue*);

void enqueue(Queue*, void*);

void dequeue(Queue*);

void* front(Queue*);

void free_queue(Queue*);

#endif