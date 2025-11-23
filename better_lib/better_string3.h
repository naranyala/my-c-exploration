
#ifndef BETTER_STRING_H
#define BETTER_STRING_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// Safe copy with size check
static inline void strcpy_safe(char *dst, size_t dst_size, const char *src) {
    if (!dst || !src || dst_size == 0) return;
    snprintf(dst, dst_size, "%s", src);
}

// Safe concatenation
static inline void strcat_safe(char *dst, size_t dst_size, const char *src) {
    if (!dst || !src || dst_size == 0) return;
    strncat(dst, src, dst_size - strlen(dst) - 1);
}

// UTF-8 length (counts codepoints, not bytes)
static inline size_t utf8_len(const char *s) {
    size_t len = 0;
    while (*s) {
        unsigned char c = (unsigned char)*s;
        if ((c & 0xC0) != 0x80) len++; // count only leading bytes
        s++;
    }
    return len;
}

#endif // BETTER_STRING_H
