// proper_align.h
#ifndef PROPER_ALIGN_H
#define PROPER_ALIGN_H

#include <stddef.h>

// Align 'size' up to 'alignment' (must be power of two)
static inline size_t proper_align_up(size_t size, size_t alignment) {
  return (size + alignment - 1) & ~(alignment - 1);
}

// Allocate aligned memory using standard malloc (fallback if no aligned_alloc)
static inline void *proper_malloc_aligned(size_t size, size_t alignment) {
  if (alignment < sizeof(void *))
    alignment = sizeof(void__);
  size_t total = size + alignment - 1 + sizeof(void *);
  char *raw = (char *)malloc(total);
  if (!raw)
    return NULL;
  char *ptr = raw + sizeof(void *);
  ptr += alignment - ((uintptr_t)ptr % alignment);
  ((void **)ptr)[-1] = raw;
  return ptr;
}

// Free memory allocated by proper_malloc_aligned
static inline void proper_free_aligned(void *ptr) {
  if (ptr) {
    free(((void **)ptr)[-1]);
  }
}

#endif // PROPER_ALIGN_H
