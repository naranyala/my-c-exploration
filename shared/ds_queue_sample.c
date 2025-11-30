
#include <stdio.h>
#include <stdlib.h>
#define MAX_SIZE 100

typedef struct Queue {
    int items[MAX_SIZE];
    int front;
    int rear;
} Queue;

// Initialize queue
void initializeQueue(Queue *q) {
    q->front = -1;
    q->rear = -1;
}

// Check if queue is full
int isFullQueue(Queue *q) {
    return q->rear == MAX_SIZE - 1;
}

// Check if queue is empty
int isEmptyQueue(Queue *q) {
    return q->front == -1;
}

// Enqueue an item
void enqueue(Queue *q, int value) {
    if (isFullQueue(q)) {
        printf("Queue is full!\n");
        return;
    }
    if (isEmptyQueue(q)) {
        q->front = 0;
    }
    q->items[++(q->rear)] = value;
}

// Dequeue an item
int dequeue(Queue *q) {
    if (isEmptyQueue(q)) {
        printf("Queue is empty!\n");
        return -1;
    }
    int value = q->items[q->front];
    if (q->front == q->rear) {
        q->front = q->rear = -1;
    } else {
        q->front++;
    }
    return value;
}
