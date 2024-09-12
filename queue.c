#include "queue.h"

#include <stdlib.h>

static Node* create_node(void* data) {
    Node* node = (Node*)malloc(sizeof(*node));
    node->data = data;
    node->next = NULL;

    return node;
}

static void free_node(Node* node, void (*free_data)(void*)) {
    if (free_data) free_data(node->data);
    node->next = NULL;
    free(node);
}

Queue* create_queue(void (*free_data)(void*)) {
    Queue* q = (Queue*)malloc(sizeof(*q));

    q->head = NULL;
    q->tail = NULL;
    q->free_data = free_data;

    return q;
}

bool is_empty(Queue* queue) {
    return queue->head == NULL;
}

void enqueue(Queue* queue, void* data) {
    Node* node = create_node((void*)data);

    if (!queue->head) {
        queue->head = node;
        queue->tail = node;
        return;
    }

    queue->tail->next = node;
}

void dequeue(Queue* queue) {
    bool empty = is_empty(queue);
    if (empty) return;

    Node* temp = queue->head;
    queue->head = queue->head->next;

    if (!queue->head) queue->tail = NULL;

    free_node(temp, queue->free_data);
}

void* front(Queue* queue) {
    if (!queue->head) return NULL;

    return queue->head->data;
}

void free_queue(Queue* queue) {
    while (!is_empty(queue)) dequeue(queue);
    free(queue);
}