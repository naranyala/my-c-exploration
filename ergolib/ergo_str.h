/* ergo_str.h
 *
 * Ergonomic, safe string utilities for C.
 *
 * Define ERGO_STR_IMPLEMENTATION in ONE C file to include the implementation.
 *
 * MIT License (or public domain) – use freely.
 */

#ifndef ERGO_STR_H
#define ERGO_STR_H

#include <stddef.h>   /* size_t */
#include <stdlib.h>   /* malloc, free */
#include <string.h>   /* strlen, strncmp, etc. */

#ifdef __cplusplus
extern "C" {
#endif

/* Safe strcpy: always null-terminates. Returns number of chars written (excluding null).
 * If dstsz == 0, does nothing. If src is NULL, treats as empty string.
 */
size_t ergo_strcpy_s(char *dst, size_t dstsz, const char *src);

/* Safe strcat: appends src to dst. Always null-terminates.
 * Returns total length of resulting string (excluding null), or dstsz if truncated.
 */
size_t ergo_strcat_s(char *dst, size_t dstsz, const char *src);

/* Null-safe string equality */
int ergo_streq(const char *s1, const char *s2);

/* Safe strdup: returns malloc'd copy or NULL on failure */
char *ergo_strdup(const char *s);

/* In-place whitespace trim (leading/trailing). Returns new start pointer (within buf). */
char *ergo_strtrim(char *buf);

#ifdef __cplusplus
}
#endif

#endif /* ERGO_STR_H */

/* ========================= IMPLEMENTATION ========================= */
#ifdef ERGO_STR_IMPLEMENTATION

#include <stdio.h>  /* for fputs in error case (optional) */
#include <errno.h>

size_t ergo_strcpy_s(char *dst, size_t dstsz, const char *src) {
    if (dstsz == 0) return 0;
    if (!dst) return 0;
    if (!src) src = "";

    size_t i;
    for (i = 0; i < dstsz - 1 && src[i] != '\0'; ++i) {
        dst[i] = src[i];
    }
    dst[i] = '\0';
    return i;
}

size_t ergo_strcat_s(char *dst, size_t dstsz, const char *src) {
    if (dstsz == 0 || !dst) return 0;
    if (!src) src = "";

    size_t len = strlen(dst);
    if (len >= dstsz) {
        dst[0] = '\0';
        return 0;
    }
    return len + ergo_strcpy_s(dst + len, dstsz - len, src);
}

int ergo_streq(const char *s1, const char *s2) {
    if (s1 == s2) return 1;
    if (!s1 || !s2) return 0;
    return strcmp(s1, s2) == 0;
}

char *ergo_strdup(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    char *p = (char *)malloc(len + 1);
    if (p) memcpy(p, s, len + 1);
    return p;
}

char *ergo_strtrim(char *buf) {
    if (!buf) return NULL;

    // Trim leading
    char *start = buf;
    while (*start && (*start == ' ' || *start == '\t' || *start == '\n' || *start == '\r')) {
        ++start;
    }

    // If all whitespace
    if (*start == '\0') {
        buf[0] = '\0';
        return buf;
    }

    // Trim trailing
    char *end = start + strlen(start) - 1;
    while (end > start && (*end == ' ' || *end == '\t' || *end == '\n' || *end == '\r')) {
        --end;
    }
    *(end + 1) = '\0';

    // Shift if needed (optional: we return new start instead)
    return start;
}

#endif /* ERGO_STR_IMPLEMENTATION */
