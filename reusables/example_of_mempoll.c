#include <stdio.h>
#include <stdlib.h>

typedef struct {
  void *memory;
  size_t total_size;
  size_t used;
  size_t block_size;
  size_t block_count;
} MemoryPool;

MemoryPool *create_pool(size_t block_count, size_t block_size) {
  MemoryPool *pool = malloc(sizeof(MemoryPool));
  pool->block_size = block_size;
  pool->block_count = block_count;
  pool->total_size = block_count * block_size;
  pool->memory = malloc(pool->total_size);
  pool->used = 0;
  return pool;
}

void *pool_alloc(MemoryPool *pool) {
  if (pool->used + pool->block_size > pool->total_size) {
    return NULL; // Pool exhausted
  }

  void *ptr = (char *)pool->memory + pool->used;
  pool->used += pool->block_size;
  return ptr;
}

void pool_reset(MemoryPool *pool) { pool->used = 0; }

void pool_free(MemoryPool *pool) {
  free(pool->memory);
  free(pool);
}

int main() {
  MemoryPool *pool = create_pool(10, sizeof(int));

  int *ptr1 = (int *)pool_alloc(pool);
  int *ptr2 = (int *)pool_alloc(pool);

  *ptr1 = 42;
  *ptr2 = 24;

  printf("ptr1: %d, ptr2: %d\n", *ptr1, *ptr2);

  pool_reset(pool); // Reset without freeing individual allocations

  pool_free(pool);
  return 0;
}
