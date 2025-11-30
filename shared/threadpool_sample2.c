/**
 * Simple thread pool implementation
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define THREAD_POOL_SIZE 4
#define QUEUE_SIZE 100

typedef struct {
  void (*function)(void *);
  void *argument;
} Task;

typedef struct {
  Task tasks[QUEUE_SIZE];
  int front, rear, count;
  pthread_mutex_t lock;
  pthread_cond_t not_empty;
  pthread_cond_t not_full;
} TaskQueue;

typedef struct {
  pthread_t threads[THREAD_POOL_SIZE];
  TaskQueue queue;
  int shutdown;
} ThreadPool;

void task_queue_init(TaskQueue *queue) {
  queue->front = queue->rear = queue->count = 0;
  pthread_mutex_init(&queue->lock, NULL);
  pthread_cond_init(&queue->not_empty, NULL);
  pthread_cond_init(&queue->not_full, NULL);
}

void task_queue_push(TaskQueue *queue, Task task) {
  pthread_mutex_lock(&queue->lock);

  while (queue->count >= QUEUE_SIZE) {
    pthread_cond_wait(&queue->not_full, &queue->lock);
  }

  queue->tasks[queue->rear] = task;
  queue->rear = (queue->rear + 1) % QUEUE_SIZE;
  queue->count++;

  pthread_cond_signal(&queue->not_empty);
  pthread_mutex_unlock(&queue->lock);
}

Task task_queue_pop(TaskQueue *queue) {
  pthread_mutex_lock(&queue->lock);

  while (queue->count <= 0) {
    pthread_cond_wait(&queue->not_empty, &queue->lock);
  }

  Task task = queue->tasks[queue->front];
  queue->front = (queue->front + 1) % QUEUE_SIZE;
  queue->count--;

  pthread_cond_signal(&queue->not_full);
  pthread_mutex_unlock(&queue->lock);

  return task;
}

void *worker_thread(void *arg) {
  ThreadPool *pool = (ThreadPool *)arg;

  while (1) {
    Task task = task_queue_pop(&pool->queue);

    if (task.function == NULL) {
      break; // Shutdown signal
    }

    task.function(task.argument);
  }

  return NULL;
}

ThreadPool *thread_pool_create(void) {
  ThreadPool *pool = malloc(sizeof(ThreadPool));
  if (!pool)
    return NULL;

  task_queue_init(&pool->queue);
  pool->shutdown = 0;

  for (int i = 0; i < THREAD_POOL_SIZE; i++) {
    pthread_create(&pool->threads[i], NULL, worker_thread, pool);
  }

  return pool;
}

void thread_pool_submit(ThreadPool *pool, void (*function)(void *), void *arg) {
  Task task = {function, arg};
  task_queue_push(&pool->queue, task);
}

void thread_pool_destroy(ThreadPool *pool) {
  pool->shutdown = 1;

  // Send shutdown tasks
  for (int i = 0; i < THREAD_POOL_SIZE; i++) {
    Task task = {NULL, NULL};
    task_queue_push(&pool->queue, task);
  }

  // Wait for threads to finish
  for (int i = 0; i < THREAD_POOL_SIZE; i++) {
    pthread_join(pool->threads[i], NULL);
  }

  free(pool);
}

// Example task function
void example_task(void *arg) {
  int *value = (int *)arg;
  printf("Processing task with value: %d (Thread: %lu)\n", *value,
         pthread_self());
  free(value);
}

// Example usage
int main(void) {
  ThreadPool *pool = thread_pool_create();

  // Submit tasks
  for (int i = 0; i < 10; i++) {
    int *value = malloc(sizeof(int));
    *value = i;
    thread_pool_submit(pool, example_task, value);
  }

  sleep(2); // Wait for tasks to complete
  thread_pool_destroy(pool);

  return 0;
}
