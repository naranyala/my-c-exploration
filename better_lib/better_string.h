/* better_string.h v1.2 - https://github.com/yourname/better-c
   Drop-in replacement and extension for <string.h>
   Public domain / CC0 */
#ifndef BETTER_STRING_H
#define BETTER_STRING_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>

#ifdef __GNUC__
#define bstr_inline static inline __attribute__((always_inline))
#else
#define bstr_inline static inline
#endif

/* Safe string view (non-owning) */
typedef struct { const char* data; size_t len; } bstr_view;

/* Create view from literal or string */
#define bstr_lit(s) (bstr_view){ .data = (s), .len = sizeof(s)-1 }
#define bstr_from_cstr(s) (bstr_view){ .data = (s), .len = (s) ? strlen(s) : 0 }

/* Null-safe wrappers */
bstr_inline char* bstr_dup(const char* s) {
    if (!s) return NULL;
    size_t n = strlen(s);
    char* p = (char*)malloc(n + 1);
    if (p) { memcpy(p, s, n); p[n] = '\0'; }
    return p;
}

bstr_inline char* bstr_ndup(const char* s, size_t n) {
    if (!s) return NULL;
    char* p = (char*)malloc(n + 1);
    if (p) { memcpy(p, s, n); p[n] = '\0'; }
    return p;
}

/* Safe copy (like strlcpy) */
bstr_inline size_t bstr_cpy(char* dst, const char* src, size_t dstsize) {
    if (!dst || !src || dstsize == 0) return 0;
    size_t n = dstsize - 1;
    size_t i = 0;
    while (i < n && src[i]) { dst[i] = src[i]; i++; }
    dst[i] = '\0';
    return i + (src[i] != '\0'); /* return needed size */
}

/* String predicates */
bstr_inline bool bstr_starts_with(const char* s, const char* prefix) {
    if (!s || !prefix) return false;
    return strncmp(s, prefix, strlen(prefix)) == 0;
}

bstr_inline bool bstr_ends_with(const char* s, const char* suffix) {
    if (!s || !suffix) return false;
    size_t sl = strlen(s), sf = strlen(suffix);
    return sl >= sf && strcmp(s + sl - sf, suffix) == 0;
}

bstr_inline bool bstr_equals(const char* a, const char* b) {
    return (!a && !b) || (a && b && strcmp(a, b) == 0);
}

/* asprintf that aborts on failure (never returns NULL) */
static inline char* bstr_aprintf(const char* fmt, ...) {
    va_list ap, ap2;
    va_start(ap, fmt);
    va_copy(ap2, ap);
    int n = vsnprintf(NULL, 0, fmt, ap);
    va_end(ap);
    if (n < 0) { perror("vsnprintf"); abort(); }
    char* buf = (char*)malloc((size_t)n + 1);
    if (!buf) { perror("malloc"); abort(); }
    vsnprintf(buf, (size_t)n + 1, fmt, ap2);
    va_end(ap2);
    return buf;
}

/* Join array of strings */
static inline char* bstr_join(const char* sep, const char** parts, size_t count) {
    if (count == 0) return bstr_dup("");
    size_t seplen = strlen(sep);
    size_t total = (count - 1) * seplen;
    for (size_t i = 0; i < count; i++) total += strlen(parts[i]);
    char* result = (char*)malloc(total + 1);
    if (!result) return NULL;
    char* p = result;
    strcpy(p, parts[0]); p += strlen(parts[0]);
    for (size_t i = 1; i < count; i++) {
        memcpy(p, sep, seplen); p += seplen;
        strcpy(p, parts[i]); p += strlen(parts[i]);
    }
    *p = '\0';
    return result;
}

#endif /* BETTER_STRING_H */
