// better_malloc.h
#ifndef BETTER_MALLOC_H
#define BETTER_MALLOC_H

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <limits.h>

#ifdef __cplusplus
extern "C" {
#endif

// Safe malloc: checks for zero size (optional) and returns NULL on failure.
void* b_malloc(size_t size);

// Safe calloc: checks for overflow in nmemb * size.
void* b_calloc(size_t nmemb, size_t size);

// Safe realloc: preserves original pointer on failure if desired (not done here for simplicity).
void* b_realloc(void* ptr, size_t new_size);

// Free (just a passthrough, but allows future hooks)
void b_free(void* ptr);

#ifdef __cplusplus
}
#endif

#ifdef BETTER_MALLOC_IMPL

#include <string.h>

void* b_malloc(size_t size) {
    if (size == 0) return NULL;  // or return malloc(1); — choice is yours
    void* p = malloc(size);
    if (!p) {
        // For production, consider returning NULL instead of aborting.
        fprintf(stderr, "b_malloc: allocation of %zu bytes failed\n", size);
        abort();
    }
    return p;
}

void* b_calloc(size_t nmemb, size_t size) {
    if (nmemb == 0 || size == 0) return NULL;
    // Overflow check
    if (nmemb > SIZE_MAX / size) {
        fprintf(stderr, "b_calloc: integer overflow in %zu * %zu\n", nmemb, size);
        abort();
    }
    void* p = calloc(nmemb, size);
    if (!p) {
        fprintf(stderr, "b_calloc: allocation failed\n");
        abort();
    }
    return p;
}

void* b_realloc(void* ptr, size_t new_size) {
    if (new_size == 0) {
        free(ptr);
        return NULL;
    }
    void* p = realloc(ptr, new_size);
    if (!p) {
        fprintf(stderr, "b_realloc: reallocation to %zu bytes failed\n", new_size);
        abort();
    }
    return p;
}

void b_free(void* ptr) {
    free(ptr);
}

#endif // BETTER_MALLOC_IMPL

#endif // BETTER_MALLOC_H
