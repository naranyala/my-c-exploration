
// tsqueue.h
#ifndef TSQUEUE_H
#define TSQUEUE_H

#include <pthread.h>
#include <stdlib.h>

typedef struct Node {
  void *data;
  struct Node *next;
} Node;

typedef struct {
  Node *head, *tail;
  pthread_mutex_t lock;
} TSQueue;

static inline TSQueue *tsqueue_new() {
  TSQueue *q = malloc(sizeof(TSQueue));
  q->head = q->tail = NULL;
  pthread_mutex_init(&q->lock, NULL);
  return q;
}

static inline void tsqueue_push(TSQueue *q, void *data) {
  Node *n = malloc(sizeof(Node));
  n->data = data;
  n->next = NULL;
  pthread_mutex_lock(&q->lock);
  if (q->tail)
    q->tail->next = n;
  else
    q->head = n;
  q->tail = n;
  pthread_mutex_unlock(&q->lock);
}

static inline void *tsqueue_pop(TSQueue *q) {
  pthread_mutex_lock(&q->lock);
  if (!q->head) {
    pthread_mutex_unlock(&q->lock);
    return NULL;
  }
  Node *n = q->head;
  q->head = n->next;
  if (!q->head)
    q->tail = NULL;
  pthread_mutex_unlock(&q->lock);
  void *data = n->data;
  free(n);
  return data;
}

#endif
