/* x_arena.h – fast temporary allocations */
#ifndef X_ARENA_H
#define X_ARENA_H
#include <stdlib.h>
#include <string.h>

typedef struct { char *ptr, *end; char **blocks; size_t nblocks, capblocks; } x_arena;

#define arena_new()             ((x_arena){0})
#define arena_alloc(a, n)       (((a)->ptr+(n) <= (a)->end) ? ((a)->ptr+=(n), (a)->ptr-(n)) : arena_grow(a,n))
#define arena_free(a)           (arena_reset(a), free((a)->blocks), *(a)=(x_arena){0})
#define arena_reset(a)          do { for (size_t i=0;i<(a)->nblocks;i++) free((a)->blocks[i]); (a)->ptr=(a)->end=0; (a)->nblocks=0; } while(0)

static inline void* arena_grow(x_arena *a, size_t n) {
    size_t sz = n < 4096 ? 4096 : n*2;
    char *p = malloc(sz);
    if (a->nblocks >= a->capblocks) {
        a->capblocks = a->capblocks ? a->capblocks*2 : 16;
        a->blocks = realloc(a->blocks, a->capblocks*sizeof(*a->blocks));
    }
    a->blocks[a->nblocks++] = p;
    a->ptr = p; a->end = p+sz;
    a->ptr += n;
    return p;
}
#endif
