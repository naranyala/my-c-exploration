/* ergo_str.h */
#pragma once
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct {
    char *data;
    size_t len;
    size_t cap;
} estr_t;

#define estr_lit(s) (estr_t){ .data = (char*)(s), .len = sizeof(s)-1, .cap = 0 }
#define estr_free(e) do { if ((e).cap) free((e).data); } while(0)

static inline estr_t estr_new(const char *s) {
    size_t n = s ? strlen(s) : 0;
    return (estr_t){ .data = s ? strdup(s) : NULL, .len = n, .cap = n+1 };
}

static inline void estr_reserve(estr_t *e, size_t need) {
    if (e->cap >= need+1) return;
    size_t newcap = e->cap ? e->cap : 16;
    while (newcap < need+1) newcap *= 2;
    e->data = realloc(e->data, newcap);
    e->cap = newcap;
}

static inline void estr_append(estr_t *e, const char *s) {
    size_t n = strlen(s);
    estr_reserve(e, e->len + n);
    memcpy(e->data + e->len, s, n);
    e->len += n;
    e->data[e->len] = '\0';
}

static inline void estr_appendf(estr_t *e, const char *fmt, ...) {
    va_list va;
    va_start(va, fmt);
    int need = vsnprintf(NULL, 0, fmt, va);
    va_end(va);
    estr_reserve(e, e->len + need);
    va_start(va, fmt);
    vsnprintf(e->data + e->len, need+1, fmt, va);
    va_end(va);
    e->len += need;
}

#define estr_cat(e, ...) do { \
    const char *parts[] = { __VA_ARGS__, NULL }; \
    for (int i = 0; parts[i]; ++i) estr_append(&(e), parts[i]); \
} while(0)

