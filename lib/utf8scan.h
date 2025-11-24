#ifndef UTF8SCAN_H
#define UTF8SCAN_H

#include <stdint.h>

typedef struct {
    const char *p;
    uint32_t cp;
    int valid;
} utf8_iter;

utf8_iter utf8_next(const char *p);

#ifdef UTF8SCAN_IMPLEMENTATION
#define U8_BAD 0xFFFD

utf8_iter utf8_next(const char *p) {
    utf8_iter it = { .p = p, .cp = U8_BAD, .valid = 0 };
    unsigned char c = (unsigned char)*p;

    if (c < 0x80) {
        it.cp = c;
        it.p = p + 1;
        it.valid = 1;
        return it;
    }

    // multi-byte decode
    uint32_t x;
    int len;

    if ((c & 0xE0) == 0xC0)      x = c & 0x1F, len = 2;
    else if ((c & 0xF0) == 0xE0) x = c & 0x0F, len = 3;
    else if ((c & 0xF8) == 0xF0) x = c & 0x07, len = 4;
    else return it;

    for (int i = 1; i < len; i++) {
        unsigned char t = (unsigned char)p[i];
        if ((t & 0xC0) != 0x80) return it;
        x = (x << 6) | (t & 0x3F);
    }

    it.cp = x;
    it.p = p + len;
    it.valid = 1;
    return it;
}

#endif // UTF8SCAN_IMPLEMENTATION
#endif

