/* ergo_str.h — small string helpers and dynamic string (DString) */
#ifndef ERGO_STR_H
#define ERGO_STR_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Trim in-place (leading & trailing whitespace). Returns same pointer. */
char *ergo_str_trim(char *s);

/* Duplicate substring [start, start+len) as malloc'd NUL-terminated string. */
char *ergo_str_dup_range(const char *s, size_t start, size_t len);

/* Dynamic string (DString) */
typedef struct {
    char *buf;
    size_t len;
    size_t cap;
} ergo_dstring;

void ergo_dstr_init(ergo_dstring *d);
void ergo_dstr_free(ergo_dstring *d);
int ergo_dstr_appendf(ergo_dstring *d, const char *fmt, ...); /* returns 0 on success */

#ifdef __cplusplus
}
#endif
#endif /* ERGO_STR_H */

#ifdef ERGO_STR_IMPLEMENTATION
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>

char *ergo_str_trim(char *s){
    if (!s) return s;
    char *end = s + strlen(s);
    while (s < end && isspace((unsigned char)*s)) s++;
    char *start = s;
    if (*start == '\0') return start;
    char *e = start + strlen(start) - 1;
    while (e > start && isspace((unsigned char)*e)) *e-- = '\0';
    /* move to beginning if we skipped leading spaces */
    if (start != s) memmove(s, start, strlen(start)+1);
    return s;
}

char *ergo_str_dup_range(const char *s, size_t start, size_t len){
    if (!s) return NULL;
    size_t sl = strlen(s);
    if (start > sl) return NULL;
    size_t avail = sl - start;
    size_t use = (len < avail) ? len : avail;
    char *r = malloc(use + 1);
    if (!r) return NULL;
    memcpy(r, s + start, use);
    r[use] = '\0';
    return r;
}

/* DString */
void ergo_dstr_init(ergo_dstring *d){
    d->buf = NULL; d->len = 0; d->cap = 0;
}
void ergo_dstr_free(ergo_dstring *d){
    free(d->buf);
    d->buf = NULL; d->len = d->cap = 0;
}
static int ergo_dstr_reserve(ergo_dstring *d, size_t want){
    if (want <= d->cap) return 0;
    size_t nc = d->cap ? d->cap * 2 : 64;
    while (nc < want) nc *= 2;
    char *nb = realloc(d->buf, nc);
    if (!nb) return -1;
    d->buf = nb; d->cap = nc;
    return 0;
}
int ergo_dstr_appendf(ergo_dstring *d, const char *fmt, ...){
    va_list ap;
    va_start(ap, fmt);
    /* measure */
    va_list ap2; va_copy(ap2, ap);
    int need = vsnprintf(NULL, 0, fmt, ap2);
    va_end(ap2);
    if (need < 0) { va_end(ap); return -1; }
    size_t want = d->len + (size_t)need + 1;
    if (ergo_dstr_reserve(d, want) != 0) { va_end(ap); return -1; }
    vsnprintf(d->buf + d->len, (size_t)need + 1, fmt, ap);
    d->len += (size_t)need;
    va_end(ap);
    return 0;
}
#endif /* ERGO_STR_IMPLEMENTATION */

