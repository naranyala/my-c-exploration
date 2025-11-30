#include <stdio.h>
#include <stdlib.h>

typedef struct {
  int *buffer;
  size_t capacity;
  size_t read_index;
  size_t write_index;
  size_t count;
} RingBuffer;

RingBuffer *ring_buffer_create(size_t capacity) {
  RingBuffer *rb = malloc(sizeof(RingBuffer));
  rb->buffer = malloc(capacity * sizeof(int));
  rb->capacity = capacity;
  rb->read_index = 0;
  rb->write_index = 0;
  rb->count = 0;
  return rb;
}

int ring_buffer_push(RingBuffer *rb, int value) {
  if (rb->count == rb->capacity) {
    return 0; // Buffer full
  }

  rb->buffer[rb->write_index] = value;
  rb->write_index = (rb->write_index + 1) % rb->capacity;
  rb->count++;
  return 1;
}

int ring_buffer_pop(RingBuffer *rb, int *value) {
  if (rb->count == 0) {
    return 0; // Buffer empty
  }

  *value = rb->buffer[rb->read_index];
  rb->read_index = (rb->read_index + 1) % rb->capacity;
  rb->count--;
  return 1;
}

int ring_buffer_is_empty(RingBuffer *rb) { return rb->count == 0; }

int ring_buffer_is_full(RingBuffer *rb) { return rb->count == rb->capacity; }

void ring_buffer_free(RingBuffer *rb) {
  free(rb->buffer);
  free(rb);
}

int main() {
  RingBuffer *rb = ring_buffer_create(5);

  // Fill buffer
  for (int i = 0; i < 5; i++) {
    ring_buffer_push(rb, i * 10);
  }

  // Try to add one more (should fail)
  if (!ring_buffer_push(rb, 999)) {
    printf("Buffer is full!\n");
  }

  // Read all values
  int value;
  while (ring_buffer_pop(rb, &value)) {
    printf("Read: %d\n", value);
  }

  ring_buffer_free(rb);
  return 0;
}
