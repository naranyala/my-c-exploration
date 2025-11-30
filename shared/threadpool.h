
// threadpool.h
#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <pthread.h>
#include <stdlib.h>

typedef struct {
  void (*func)(void *);
  void *arg;
} Task;

typedef struct {
  pthread_t *threads;
  size_t count;
} ThreadPool;

static inline void *worker(void *arg) {
  Task *task = (Task *)arg;
  task->func(task->arg);
  free(task);
  return NULL;
}

static inline ThreadPool *threadpool_new(size_t n) {
  ThreadPool *pool = malloc(sizeof(ThreadPool));
  pool->threads = malloc(sizeof(pthread_t) * n);
  pool->count = n;
  return pool;
}

static inline void threadpool_submit(ThreadPool *pool, void (*func)(void *),
                                     void *arg) {
  Task *task = malloc(sizeof(Task));
  task->func = func;
  task->arg = arg;
  pthread_create(&pool->threads[rand() % pool->count], NULL, worker, task);
}

#endif
