/* ergo_arena.h */
#pragma once
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *begin;
    char *end;
    char *cur;
    struct arena_chunk *next;
} ergo_arena_t;

typedef struct arena_chunk {
    struct arena_chunk *next;
    char data[];  /* flexible array */
} arena_chunk_t;

#define ARENA_CHUNK_SIZE (64*1024*1024)  /* 64 MiB default chunks */

static inline void arena_init(ergo_arena_t *a) { *a = (ergo_arena_t){0}; }

static inline void* arena_alloc(ergo_arena_t *a, size_t size, size_t align) {
    size_t offset = (size_t)(a->cur) & (align-1);
    if (offset) a->cur += align - offset;

    if (a->cur + size > a->end) {
        size_t chunk_size = sizeof(arena_chunk_t) + (size > ARENA_CHUNK_SIZE ? size : ARENA_CHUNK_SIZE);
        arena_chunk_t *chunk = aligned_alloc(align, chunk_size);
        chunk->next = a->next;
        a->next = chunk;
        a->begin = a->cur = chunk->data;
        a->end = (char*)chunk + chunk_size;
    }
    void *p = a->cur;
    a->cur += size;
    return p;
}

#define arena_new(a, T)            ((T*)arena_alloc(a, sizeof(T), _Alignof(T)))
#define arena_strndup(a, s, n)     ({ char *d = arena_alloc(a, n+1, 1); memcpy(d, s, n); d[n]=0; d; })
#define arena_strdup(a, s)         arena_strndup(a, s, strlen(s))

static inline void arena_reset(ergo_arena_t *a) {
    a->cur = a->begin;
}

static inline void arena_free(ergo_arena_t *a) {
    arena_chunk_t *c = a->next;
    while (c) {
        arena_chunk_t *next = c->next;
        free(c);
        c = next;
    }
    *a = (ergo_arena_t){0};
}

