// proper_queue.h
#ifndef PROPER_QUEUE_H
#define PROPER_QUEUE_H

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// SPSC (single-producer, single-consumer) queue for POD types
// Not thread-safe for MP/MC!
typedef struct {
  void *buffer;
  size_t elem_size;
  size_t capacity; // always power of two
  volatile uint32_t head;
  volatile uint32_t tail;
} proper_queue_t;

static inline int proper_queue_init(proper_queue_t *q, size_t elem_size,
                                    size_t count) {
  // Round count up to next power of two
  size_t cap = 1;
  while (cap < count)
    cap <<= 1;
  q->buffer = malloc(cap * elem_size);
  if (!q->buffer)
    return -1;
  q->elem_size = elem_size;
  q->capacity = cap;
  q->head = 0;
  q->tail = 0;
  return 0;
}

static inline void proper_queue_destroy(proper_queue_t *q) {
  free(q->buffer);
  q->buffer = NULL;
}

static inline int proper_queue_push(proper_queue_t *q, const void *item) {
  uint32_t head = q->head;
  uint32_t next_head = (head + 1) & (uint32_t)(q->capacity - 1);
  if (next_head == q->tail)
    return -1; // full
  memcpy((char *)q->buffer + head * q->elem_size, item, q->elem_size);
  q->head = next_head;
  return 0;
}

static inline int proper_queue_pop(proper_queue_t *q, void *item) {
  uint32_t tail = q->tail;
  if (tail == q->head)
    return -1; // empty
  memcpy(item, (char *)q->buffer + tail * q->elem_size, q->elem_size);
  q->tail = (tail + 1) & (uint32_t)(q->capacity - 1);
  return 0;
}

static inline int proper_queue_empty(const proper_queue_t *q) {
  return q->head == q->tail;
}

static inline size_t proper_queue_count(const proper_queue_t *q) {
  uint32_t h = q->head, t = q->tail;
  return (h >= t) ? (h - t) : (q->capacity - t + h);
}

#endif // PROPER_QUEUE_H
