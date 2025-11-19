/**
 * Circular buffer/ring buffer implementation
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  int *buffer;
  size_t head;
  size_t tail;
  size_t size;
  size_t capacity;
  bool full;
} CircularBuffer;

CircularBuffer *circular_buffer_create(size_t capacity) {
  CircularBuffer *cb = malloc(sizeof(CircularBuffer));
  if (!cb)
    return NULL;

  cb->buffer = malloc(capacity * sizeof(int));
  if (!cb->buffer) {
    free(cb);
    return NULL;
  }

  cb->head = 0;
  cb->tail = 0;
  cb->size = 0;
  cb->capacity = capacity;
  cb->full = false;
  return cb;
}

void circular_buffer_destroy(CircularBuffer *cb) {
  if (cb) {
    free(cb->buffer);
    free(cb);
  }
}

bool circular_buffer_is_empty(const CircularBuffer *cb) {
  return !cb->full && (cb->head == cb->tail);
}

bool circular_buffer_is_full(const CircularBuffer *cb) { return cb->full; }

size_t circular_buffer_size(const CircularBuffer *cb) {
  if (cb->full)
    return cb->capacity;

  if (cb->head >= cb->tail) {
    return cb->head - cb->tail;
  }
  return cb->capacity - cb->tail + cb->head;
}

bool circular_buffer_put(CircularBuffer *cb, int value) {
  if (circular_buffer_is_full(cb)) {
    return false;
  }

  cb->buffer[cb->head] = value;
  cb->head = (cb->head + 1) % cb->capacity;
  cb->size++;

  if (cb->head == cb->tail) {
    cb->full = true;
  }

  return true;
}

bool circular_buffer_get(CircularBuffer *cb, int *value) {
  if (circular_buffer_is_empty(cb)) {
    return false;
  }

  *value = cb->buffer[cb->tail];
  cb->tail = (cb->tail + 1) % cb->capacity;
  cb->size--;
  cb->full = false;

  return true;
}

void circular_buffer_reset(CircularBuffer *cb) {
  cb->head = 0;
  cb->tail = 0;
  cb->size = 0;
  cb->full = false;
}

// Example usage
int main(void) {
  CircularBuffer *cb = circular_buffer_create(5);

  // Fill the buffer
  for (int i = 1; i <= 6; i++) {
    if (circular_buffer_put(cb, i * 10)) {
      printf("Added: %d\n", i * 10);
    } else {
      printf("Buffer full, couldn't add: %d\n", i * 10);
    }
  }

  printf("Buffer size: %zu\n", circular_buffer_size(cb));

  // Read from buffer
  int value;
  while (circular_buffer_get(cb, &value)) {
    printf("Read: %d\n", value);
  }

  circular_buffer_destroy(cb);
  return 0;
}
