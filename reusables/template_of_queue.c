
#include <stdio.h>
#include <stdlib.h>

#define MAX 100

typedef struct {
  int arr[MAX];
  int front, rear;
} Queue;

void init(Queue *q) { q->front = q->rear = -1; }

int isEmpty(Queue *q) { return q->front == -1; }

int isFull(Queue *q) { return q->rear == MAX - 1; }

void enqueue(Queue *q, int val) {
  if (isFull(q))
    return;
  if (isEmpty(q))
    q->front = 0;
  q->arr[++(q->rear)] = val;
}

int dequeue(Queue *q) {
  if (isEmpty(q))
    return -1;
  int val = q->arr[q->front];
  if (q->front == q->rear)
    q->front = q->rear = -1;
  else
    q->front++;
  return val;
}

int main() {
  Queue q;
  init(&q);

  enqueue(&q, 5);
  enqueue(&q, 10);
  printf("Dequeued: %d\n", dequeue(&q));

  return 0;
}
