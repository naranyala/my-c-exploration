#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct {
  volatile long count;
  pthread_mutex_t mutex;
} ThreadSafeCounter;

ThreadSafeCounter *counter_create() {
  ThreadSafeCounter *counter = malloc(sizeof(ThreadSafeCounter));
  counter->count = 0;
  pthread_mutex_init(&counter->mutex, NULL);
  return counter;
}

void counter_increment(ThreadSafeCounter *counter) {
  pthread_mutex_lock(&counter->mutex);
  counter->count++;
  pthread_mutex_unlock(&counter->mutex);
}

long counter_get_value(ThreadSafeCounter *counter) {
  pthread_mutex_lock(&counter->mutex);
  long value = counter->count;
  pthread_mutex_unlock(&counter->mutex);
  return value;
}

void counter_free(ThreadSafeCounter *counter) {
  pthread_mutex_destroy(&counter->mutex);
  free(counter);
}

// Example usage in a multi-threaded context would use pthread_create
// For demonstration, showing basic usage:
int main() {
  ThreadSafeCounter *counter = counter_create();

  // Simulate concurrent increments
  for (int i = 0; i < 1000; i++) {
    counter_increment(counter);
  }

  printf("Final count: %ld\n", counter_get_value(counter));

  counter_free(counter);
  return 0;
}
