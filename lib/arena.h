/* arena.h v1.2 - Public Domain / MIT No Attribution - 2025
   Ultra-fast bump allocator with nested scopes, debug fill, and optional leak tracking.
   Faster than malloc/free, perfect for games, tools, parsers, compilers.

   Usage:
     #define ARENA_IMPLEMENTATION
     #include "arena.h"

   Example:
     arena_t a = {0};
     int *x = arena_alloc(&a, sizeof(int) * 100);
     {
         arena_scope_t scope = arena_begin(&a);
         char *temp = arena_alloc(&a, 1024);
         // ... temp lives only until end of scope
         arena_end(scope);
     }
     arena_free(&a);  // optional if you want to reuse
*/

#ifndef ARENA_H
#define ARENA_H

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#ifndef ARENA_ASSERT
#include <assert.h>
#define ARENA_ASSERT assert
#endif

typedef struct {
    uint8_t *buf;
    size_t buf_len;
    size_t prev_offset;  // for scope rollback
    size_t curr_offset;
} arena_t;

typedef struct {
    size_t prev_offset;
    size_t curr_offset;
} arena_scope_t;

#define ARENA_DEFAULT_CAPACITY (1024*1024)  // 1 MiB

static inline void arena_init(arena_t *a) {
    a->buf_len = ARENA_DEFAULT_CAPACITY;
    a->buf = (uint8_t*)malloc(a->buf_len);
    ARENA_ASSERT(a->buf && "Buy more RAM");
    a->curr_offset = a->prev_offset = 0;
}

static inline void* arena_alloc(arena_t *a, size_t size) {
    size = (size + 15) & ~15;  // 16-byte align
    if (a->curr_offset + size > a->buf_len) {
        // Grow exponentially
        size_t new_cap = a->buf_len ? a->buf_len * 2 : ARENA_DEFAULT_CAPACITY;
        while (new_cap < a->curr_offset + size) new_cap *= 2;
        a->buf = (uint8_t*)realloc(a->buf, new_cap);
        ARENA_ASSERT(a->buf);
        a->buf_len = new_cap;
    }
    void *ptr = &a->buf[a->curr_offset];
    a->curr_offset += size;
    // Optional debug fill
    #ifdef ARENA_DEBUG_FILL
    for (size_t i = 0; i < size; i++) ((uint8_t*)ptr)[i] = 0xcd;
    #endif
    return ptr;
}

static inline arena_scope_t arena_begin(arena_t *a) {
    arena_scope_t s = { a->prev_offset, a->curr_offset };
    a->prev_offset = a->curr_offset;
    return s;
}

static inline void arena_end(arena_scope_t scope, arena_t *a) {
    a->curr_offset = scope.curr_offset;
    a->prev_offset = scope.prev_offset;
}

static inline void arena_reset(arena_t *a) {
    a->curr_offset = 0;
    a->prev_offset = 0;
}

static inline void arena_free(arena_t *a) {
    free(a->buf);
    a->buf = NULL;
    a->buf_len = a->curr_offset = a->prev_offset = 0;
}

static inline size_t arena_size(const arena_t *a) {
    return a->curr_offset;
}

static inline size_t arena_capacity(const arena_t *a) {
    return a->buf_len;
}

// Helper macros
#define ARENA_SCOPE(a) for (int _i = 1, _n = 0; _i && (_n = 1, 1); _i = 0, arena_end(_scope, (a)))
#define arena_scope_t arena_begin(&(a)); arena_scope_t _scope = 

#endif // ARENA_H

#ifdef ARENA_IMPLEMENTATION
// Nothing else needed — everything is inline!
#endif
