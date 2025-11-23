/* ergo_utf8.h */
#pragma once
#include <stddef.h>

static inline int utf8_len(const char *s) {
    int len = 0;
    while (*s) { len += (*s++ & 0xC0) != 0x80; }
    return len;
}

static inline const char* utf8_next(const char *s) {
    while ((*++s & 0xC0) == 0x80);
    return s;
}

static inline int utf8_valid(const char *s) {
    while (*s) {
        if ((*s & 0x80) == 0) { s++; }
        else if ((*s & 0xE0) == 0xC0) { if ((s[1] & 0xC0) != 0x80) return 0; s += 2; }
        else if ((*s & 0xF0) == 0xE0) { if ((s[1] & 0xC0) != 0x80 || (s[2] & 0xC0) != 0x80) return 0; s += 3; }
        else if ((*s & 0xF8) == 0xF0) { if ((s[1] & 0xC0) != 0x80 || (s[2] & 0xC0) != 0x80 || (s[3] & 0xC0) != 0x80) return 0; s += 4; }
        else return 0;
    }
    return 1;
}

