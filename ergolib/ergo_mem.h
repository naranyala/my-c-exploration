/* ergo_mem.h
 * Safe memory helpers that abort on allocation failure (fail-fast).
 * Optional: customize behavior via ERGO_MEM_ABORT_FN.
 */

#ifndef ERGO_MEM_H
#define ERGO_MEM_H

#include <stddef.h>  /* size_t */

#ifdef __cplusplus
extern "C" {
#endif

/* Allocates sz bytes. Aborts on failure. */
void *ergo_malloc(size_t sz);

/* Allocates n * sz bytes, zero-initialized. Aborts on failure. */
void *ergo_calloc(size_t n, size_t sz);

/* Resizes memory. Aborts on failure. */
void *ergo_realloc(void *ptr, size_t sz);

/* Frees and (optionally) nulls the pointer */
void ergo_free(void *ptr);

/* Duplicates memory block */
void *ergo_memdup(const void *src, size_t sz);

/* Optional: define your own abort handler before including */
#ifndef ERGO_MEM_ABORT_FN
  #include <stdlib.h>
  #define ERGO_MEM_ABORT_FN abort
#endif

#ifdef __cplusplus
}
#endif

#endif /* ERGO_MEM_H */

#ifdef ERGO_MEM_IMPLEMENTATION

#include <stdlib.h>
#include <string.h>

static void ergo_mem_handle_fail(void) {
    ERGO_MEM_ABORT_FN();
}

void *ergo_malloc(size_t sz) {
    if (sz == 0) return NULL;
    void *p = malloc(sz);
    if (!p) ergo_mem_handle_fail();
    return p;
}

void *ergo_calloc(size_t n, size_t sz) {
    if (n == 0 || sz == 0) return NULL;
    void *p = calloc(n, sz);
    if (!p) ergo_mem_handle_fail();
    return p;
}

void *ergo_realloc(void *ptr, size_t sz) {
    void *p = realloc(ptr, sz);
    if (sz && !p) ergo_mem_handle_fail();
    return p;
}

void ergo_free(void *ptr) {
    free(ptr);
}

void *ergo_memdup(const void *src, size_t sz) {
    if (!src || sz == 0) return NULL;
    void *p = ergo_malloc(sz);
    memcpy(p, src, sz);
    return p;
}

#endif /* ERGO_MEM_IMPLEMENTATION */
