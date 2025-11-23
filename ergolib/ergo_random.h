/* ergo_random.h */
#pragma once
#include <stddef.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/random.h>
#include <stdlib.h>

static inline void rnd_bytes(void *buf, size_t n) {
    ssize_t got;
    while (n) {
        got = getrandom(buf, n, 0);
        if (got <= 0) { arc4random_buf(buf, n); return; }
        buf = (char*)buf + got;
        n -= got;
    }
}

static inline uint64_t rnd_u64(void) {
    uint64_t x;
    rnd_bytes(&x, sizeof(x));
    return x;
}

static inline int rnd_int(int min, int max) {
    return min + (int)(rnd_u64() % (max - min + 1));
}

