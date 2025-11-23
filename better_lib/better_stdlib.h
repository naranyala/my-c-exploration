/* better_stdlib.h v1.0 - malloc that never returns NULL */
#ifndef BETTER_STDLIB_H
#define BETTER_STDLIB_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static inline void* bmalloc(size_t size) {
    void* p = malloc(size);
    if (!p) { perror("malloc"); abort(); }
    return p;
}

static inline void* bcalloc(size_t n, size_t size) {
    void* p = calloc(n, size);
    if (!p) { perror("calloc"); abort(); }
    return p;
}

static inline void* brealloc(void* ptr, size_t size) {
    void* p = realloc(ptr, size);
    if (!p && size != 0) { perror("realloc"); abort(); }
    return p;
}

static inline char* bstrdup(const char* s) {
    char* p = strdup(s);
    if (!p) { perror("strdup"); abort(); }
    return p;
}

/* Optional: define macros to replace standard ones in your project */
#define malloc  DO_NOT_USE_MALLOC_DIRECTLY_USE_bmalloc
#define calloc  DO_NOT_USE_CALLOC_DIRECTLY_USE_bcalloc
#define realloc DO_NOT_USE_REALLOC_DIRECTLY_USE_brealloc
#define strdup  DO_NOT_USE_STRDUP_DIRECTLY_USE_bstrdup

#endif /* BETTER_STDLIB_H */
