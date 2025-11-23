// better_string.h
#ifndef BETTER_STRING_H
#define BETTER_STRING_H

#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

// Safer strncpy: always null-terminates.
char* b_strncpy(char* dest, const char* src, size_t dest_size);

// Safer strncat: respects total buffer size.
char* b_strncat(char* dest, const char* src, size_t dest_size);

// Portable asprintf-like: returns heap-allocated string, use b_free() to release.
// Returns -1 on error, 0 on success.
int b_asprintf(char** out, const char* fmt, ...);

// Split string by delimiter (returns NULL-terminated array of strings)
// Caller must call b_strsplit_free().
char** b_strsplit(const char* str, const char* delim, size_t* out_count);

// Free result of b_strsplit
void b_strsplit_free(char** arr, size_t count);

#ifdef __cplusplus
}
#endif

#ifdef BETTER_STRING_IMPL

#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>

char* b_strncpy(char* dest, const char* src, size_t dest_size) {
    if (dest_size == 0) return dest;
    size_t len = strlen(src);
    if (len >= dest_size) {
        memcpy(dest, src, dest_size - 1);
        dest[dest_size - 1] = '\0';
    } else {
        memcpy(dest, src, len + 1);
    }
    return dest;
}

char* b_strncat(char* dest, const char* src, size_t dest_size) {
    size_t dlen = strlen(dest);
    if (dlen >= dest_size) return dest;
    return b_strncpy(dest + dlen, src, dest_size - dlen);
}

int b_asprintf(char** out, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int size = vsnprintf(NULL, 0, fmt, args);
    va_end(args);
    if (size < 0) {
        *out = NULL;
        return -1;
    }
    *out = (char*)malloc((size_t)size + 1);
    if (!*out) return -1;
    va_start(args, fmt);
    int res = vsnprintf(*out, (size_t)size + 1, fmt, args);
    va_end(args);
    if (res < 0) {
        free(*out);
        *out = NULL;
        return -1;
    }
    return 0;
}

char** b_strsplit(const char* str, const char* delim, size_t* out_count) {
    if (!str || !delim) {
        if (out_count) *out_count = 0;
        return NULL;
    }
    char* s = strdup(str); // modifies copy
    if (!s) return NULL;

    size_t capacity = 8;
    size_t count = 0;
    char** arr = (char**)malloc(sizeof(char*) * capacity);
    if (!arr) { free(s); return NULL; }

    char* token = strtok(s, delim);
    while (token) {
        if (count >= capacity) {
            capacity *= 2;
            char** tmp = (char**)realloc(arr, sizeof(char*) * capacity);
            if (!tmp) goto fail;
            arr = tmp;
        }
        arr[count] = strdup(token);
        if (!arr[count]) goto fail;
        count++;
        token = strtok(NULL, delim);
    }

    free(s);
    if (out_count) *out_count = count;
    return arr;

fail:
    for (size_t i = 0; i < count; ++i) free(arr[i]);
    free(arr);
    free(s);
    if (out_count) *out_count = 0;
    return NULL;
}

void b_strsplit_free(char** arr, size_t count) {
    if (!arr) return;
    for (size_t i = 0; i < count; ++i) {
        free(arr[i]);
    }
    free(arr);
}

#endif // BETTER_STRING_IMPL

#endif // BETTER_STRING_H
