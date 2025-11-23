
/* better_arena.h */

#ifndef BETTER_ARENA_H
#define BETTER_ARENA_H

#include <stdlib.h>
#include <string.h>

typedef struct {
    unsigned char *base;
    size_t cap;
    size_t pos;
} barena;

static inline void barena_init(barena *a, size_t cap) {
    a->base = (unsigned char*)malloc(cap);
    a->cap = cap;
    a->pos = 0;
}

static inline void barena_free(barena *a) {
    free(a->base);
    a->base = 0;
    a->cap = a->pos = 0;
}

static inline void *barena_alloc(barena *a, size_t sz) {
    if (a->pos + sz > a->cap) return NULL;
    void *p = a->base + a->pos;
    a->pos += sz;
    return p;
}

static inline void barena_reset(barena *a) {
    a->pos = 0;
}

#endif
