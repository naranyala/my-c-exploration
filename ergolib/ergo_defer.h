/* ergo_defer.h - RAII-like Cleanup Mechanism */
#ifndef ERGO_DEFER_H
#define ERGO_DEFER_H

#include <stdlib.h>
#include <stdio.h>

// --- Cleanup functions (must take a pointer to the resource) ---

// For use with malloc/calloc
static inline void ergo_free_resource(void *p) {
    // p is a pointer to the variable's address (void**)
    free(*(void**)p);
}

// For use with fopen
static inline void ergo_close_file(FILE **f) {
    if (*f) {
        fclose(*f);
    }
}

// --- The DEFER macro ---
// Creates a temporary variable with the cleanup attribute.
// The temporary variable's lifetime is tied to the scope.
#define DEFER(cleanup_func, var) \
    __attribute__((cleanup(cleanup_func))) __typeof__(var) *temp_cleanup_##var = &(var)

#endif // ERGO_DEFER_H

#ifdef ERGO_IMPLEMENTATION
// This header is mostly header-only due to 'static inline' functions.
// No significant implementation code needed here.
#endif
