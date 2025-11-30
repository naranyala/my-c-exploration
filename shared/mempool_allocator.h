
// mempool.h
#ifndef MEMPOOL_H
#define MEMPOOL_H

#include <stdlib.h>

typedef struct {
  char *pool;
  size_t size;
  size_t used;
} MemPool;

static inline MemPool *mempool_new(size_t size) {
  MemPool *mp = malloc(sizeof(MemPool));
  mp->pool = malloc(size);
  mp->size = size;
  mp->used = 0;
  return mp;
}

static inline void *mempool_alloc(MemPool *mp, size_t n) {
  if (mp->used + n > mp->size)
    return NULL;
  void *ptr = mp->pool + mp->used;
  mp->used += n;
  return ptr;
}

static inline void mempool_free(MemPool *mp) {
  free(mp->pool);
  free(mp);
}

#endif
