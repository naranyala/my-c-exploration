// better_queue.h
#ifndef BETTER_QUEUE_H
#define BETTER_QUEUE_H

#include <stdatomic.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct b_queue {
    void** buffer;
    size_t capacity;
    _Atomic size_t head; // read index
    _Atomic size_t tail; // write index
} b_queue_t;

// Create queue with power-of-two capacity
b_queue_t* b_queue_create(size_t capacity_power_of_two);

// Destroy queue
void b_queue_destroy(b_queue_t* q);

// Push (single producer): returns true on success
bool b_queue_push(b_queue_t* q, void* item);

// Pop (single consumer): returns item or NULL if empty
void* b_queue_pop(b_queue_t* q);

#ifdef __cplusplus
}
#endif

#ifdef BETTER_QUEUE_IMPL

#include <stdlib.h>

static inline size_t b_queue_next_pow2(size_t v) {
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v++;
    return v;
}

b_queue_t* b_queue_create(size_t cap) {
    if (cap == 0) return NULL;
    size_t capacity = b_queue_next_pow2(cap);
    b_queue_t* q = (b_queue_t*)malloc(sizeof(b_queue_t));
    if (!q) return NULL;
    q->buffer = (void**)calloc(capacity, sizeof(void*));
    if (!q->buffer) { free(q); return NULL; }
    q->capacity = capacity;
    atomic_init(&q->head, 0);
    atomic_init(&q->tail, 0);
    return q;
}

void b_queue_destroy(b_queue_t* q) {
    if (q) {
        free(q->buffer);
        free(q);
    }
}

bool b_queue_push(b_queue_t* q, void* item) {
    size_t current_tail = atomic_load_explicit(&q->tail, memory_order_relaxed);
    size_t current_head = atomic_load_explicit(&q->head, memory_order_acquire);
    size_t next_tail = (current_tail + 1) & (q->capacity - 1);

    if (next_tail == current_head) return false; // full

    q->buffer[current_tail] = item;
    atomic_store_explicit(&q->tail, next_tail, memory_order_release);
    return true;
}

void* b_queue_pop(b_queue_t* q) {
    size_t current_head = atomic_load_explicit(&q->head, memory_order_relaxed);
    size_t current_tail = atomic_load_explicit(&q->tail, memory_order_acquire);

    if (current_head == current_tail) return NULL; // empty

    void* item = q->buffer[current_head];
    size_t next_head = (current_head + 1) & (q->capacity - 1);
    atomic_store_explicit(&q->head, next_head, memory_order_release);
    return item;
}

#endif // BETTER_QUEUE_IMPL

#endif // BETTER_QUEUE_H
