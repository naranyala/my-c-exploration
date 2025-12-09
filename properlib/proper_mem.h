// proper_mem.h
#ifndef PROPER_MEM_H
#define PROPER_MEM_H

#include <stdlib.h>
#include <string.h>

// Safe malloc: exits on failure or returns NULL if you define
// PROPER_MEM_NO_ABORT
#ifndef PROPER_MEM_NO_ABORT
#include <stdio.h>
#include <stdlib.h>
static inline void *proper_malloc(size_t size) {
  void *p = malloc(size);
  if (!p && size) {
    fprintf(stderr, "proper_malloc: allocation of %zu bytes failed\n", size);
    abort();
  }
  return p;
}
#else
static inline void *proper_malloc(size_t size) { return malloc(size); }
#endif

// Calloc wrapper
static inline void *proper_calloc(size_t n, size_t size) {
  void *p = calloc(n, size);
  if (!p && n && size) {
#ifndef PROPER_MEM_NO_ABORT
    fprintf(stderr, "proper_calloc: allocation of %zu * %zu bytes failed\n", n,
            size);
    abort();
#endif
  }
  return p;
}

// Realloc with null-safe resize
static inline void *proper_realloc(void *ptr, size_t new_size) {
  void *p = realloc(ptr, new_size);
  if (!p && new_size) {
#ifndef PROPER_MEM_NO_ABORT
    fprintf(stderr, "proper_realloc: failed to resize to %zu bytes\n",
            new_size);
    abort();
#endif
  }
  return p;
}

// Free and nullify pointer
static inline void proper_free(void **ptr) {
  if (ptr && *ptr) {
    free(*ptr);
    *ptr = NULL;
  }
}

#endif // PROPER_MEM_H
