/**
 * Memory pool allocator for efficient fixed-size allocations
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct MemoryBlock {
  struct MemoryBlock *next;
} MemoryBlock;

typedef struct {
  size_t block_size;
  size_t blocks_per_chunk;
  MemoryBlock *free_list;
} MemoryPool;

MemoryPool *memory_pool_create(size_t block_size, size_t blocks_per_chunk) {
  MemoryPool *pool = malloc(sizeof(MemoryPool));
  if (!pool)
    return NULL;

  pool->block_size =
      block_size < sizeof(MemoryBlock) ? sizeof(MemoryBlock) : block_size;
  pool->blocks_per_chunk = blocks_per_chunk;
  pool->free_list = NULL;

  return pool;
}

void *memory_pool_alloc(MemoryPool *pool) {
  if (!pool->free_list) {
    // Allocate new chunk
    size_t chunk_size = pool->block_size * pool->blocks_per_chunk;
    MemoryBlock *chunk = malloc(chunk_size);
    if (!chunk)
      return NULL;

    // Add all blocks in chunk to free list
    for (size_t i = 0; i < pool->blocks_per_chunk; i++) {
      MemoryBlock *block =
          (MemoryBlock *)((char *)chunk + i * pool->block_size);
      block->next = pool->free_list;
      pool->free_list = block;
    }
  }

  MemoryBlock *block = pool->free_list;
  pool->free_list = block->next;

  return block;
}

void memory_pool_free(MemoryPool *pool, void *ptr) {
  MemoryBlock *block = (MemoryBlock *)ptr;
  block->next = pool->free_list;
  pool->free_list = block;
}

void memory_pool_destroy(MemoryPool *pool) {
  // Note: This simple implementation doesn't track chunks for cleanup
  // In production, you'd want to track all allocated chunks
  free(pool);
}

// Example usage
int main(void) {
  MemoryPool *pool = memory_pool_create(sizeof(int), 10);

  int *values[20];

  // Allocate from pool
  for (int i = 0; i < 20; i++) {
    values[i] = memory_pool_alloc(pool);
    *values[i] = i * 10;
  }

  // Use values
  for (int i = 0; i < 20; i++) {
    printf("values[%d] = %d\n", i, *values[i]);
  }

  // Free back to pool
  for (int i = 0; i < 20; i++) {
    memory_pool_free(pool, values[i]);
  }

  memory_pool_destroy(pool);
  return 0;
}
