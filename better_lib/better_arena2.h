/* better_arena.h v1.0 - Stack-like allocator, zero overhead, zero leaks */
#ifndef BETTER_ARENA_H
#define BETTER_ARENA_H
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char* begin; char* end; char* cur; } barena;

#define BARINA_DEFAULT_SIZE (64*1024*1024UL)  // 64 MiB

static inline barena barena_create(size_t size) {
    char* mem = (char*)malloc(size ? size : BARINA_DEFAULT_SIZE);
    return (barena){ .begin = mem, .cur = mem, .end = mem + size };
}

static inline void* barena_alloc(barena* a, size_t size, size_t align) {
    size_t offset = (size_t)(a->cur - a->begin);
    offset = (offset + align - 1) & ~(align - 1);
    char* p = a->begin + offset;
    if (p + size > a->end) return NULL;  // OOM → caller decides
    a->cur = p + size;
    return p;
}

#define barena_new(a, type)            ((type*)barena_alloc(a, sizeof(type), _Alignof(type)))
#define barena_array(a, type, count)   ((type*)barena_alloc(a, sizeof(type)*(count), _Alignof(type)))

static inline void barena_reset(barena* a) { a->cur = a->begin; }
static inline void barena_destroy(barena* a) { free(a->begin); a->begin = a->cur = a->end = NULL; }

#endif
