
#ifndef BETTER_MALLOC_H
#define BETTER_MALLOC_H

#include <stdlib.h>
#include <stdio.h>

// Checked malloc
static inline void* malloc_or_die(size_t size) {
    void *p = malloc(size);
    if (!p) {
        fprintf(stderr, "malloc failed for %zu bytes\n", size);
        exit(EXIT_FAILURE);
    }
    return p;
}

// Zero-initialized allocation
static inline void* calloc_or_die(size_t n, size_t size) {
    void *p = calloc(n, size);
    if (!p) {
        fprintf(stderr, "calloc failed for %zu elements\n", n);
        exit(EXIT_FAILURE);
    }
    return p;
}

#endif // BETTER_MALLOC_H
