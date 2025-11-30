
#include <stdio.h>
#include <stdlib.h>

typedef struct MemoryBlock {
    void *memory;
    size_t blockSize;
    size_t numBlocks;
    int *freeList;
} MemoryBlock;

// Initialize a memory pool
void initializeMemoryPool(MemoryBlock *pool, size_t blockSize, size_t numBlocks) {
    pool->memory = malloc(blockSize * numBlocks);
    pool->blockSize = blockSize;
    pool->numBlocks = numBlocks;
    pool->freeList = (int *)malloc(numBlocks * sizeof(int));

    for (size_t i = 0; i < numBlocks; i++) {
        pool->freeList[i] = 1; // 1 means free
    }
}

// Allocate a block from the memory pool
void *allocateBlock(MemoryBlock *pool) {
    for (size_t i = 0; i < pool->numBlocks; i++) {
        if (pool->freeList[i] == 1) {
            pool->freeList[i] = 0; // Mark as used
            return (char *)pool->memory + (i * pool->blockSize);
        }
    }
    return NULL; // No free blocks
}

// Free a block in the memory pool
void freeBlock(MemoryBlock *pool, void *block) {
    size_t index = ((char *)block - (char *)pool->memory) / pool->blockSize;
    if (index < pool->numBlocks) {
        pool->freeList[index] = 1; // Mark as free
    }
}
