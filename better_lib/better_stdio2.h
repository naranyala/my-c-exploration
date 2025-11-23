// better_stdio.h
#ifndef BETTER_STDIO_H
#define BETTER_STDIO_H

#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

// Opens file, aborts (or returns NULL) if fails.
// Intended for scripts/prototypes where failure = bug.
FILE* b_fopen_or_abort(const char* path, const char* mode);

// Opens file, returns NULL on error (no abort).
FILE* b_fopen_safe(const char* path, const char* mode);

// Auto-close FILE* using GCC/Clang cleanup (optional)
// Usage: BFOPEN_AUTO(fp, "file.txt", "r") { ... }
#if defined(__GNUC__) || defined(__clang__)
#define BFOPEN_AUTO(fp, path, mode) \
    __attribute__((cleanup(b_fclosep))) FILE* fp = b_fopen_safe((path), (mode))
void b_fclosep(FILE** fp);
#endif

#ifdef __cplusplus
}
#endif

#ifdef BETTER_STDIO_IMPL

#include <errno.h>
#include <string.h>

FILE* b_fopen_or_abort(const char* path, const char* mode) {
    if (!path || !mode) {
        fprintf(stderr, "b_fopen_or_abort: NULL path or mode\n");
        abort();
    }
    FILE* f = fopen(path, mode);
    if (!f) {
        fprintf(stderr, "Failed to open '%s' with mode '%s': %s\n",
                path, mode, strerror(errno));
        abort();
    }
    return f;
}

FILE* b_fopen_safe(const char* path, const char* mode) {
    if (!path || !mode) return NULL;
    return fopen(path, mode);
}

#if defined(__GNUC__) || defined(__clang__)
void b_fclosep(FILE** fp) {
    if (fp && *fp) {
        fclose(*fp);
        *fp = NULL;
    }
}
#endif

#endif // BETTER_STDIO_IMPL

#endif // BETTER_STDIO_H
