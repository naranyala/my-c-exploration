
#ifndef BETTER_UNISTD_H
#define BETTER_UNISTD_H

#include <unistd.h>
#include <errno.h>

// Retry read/write until success or fatal error
static inline ssize_t safe_read(int fd, void *buf, size_t count) {
    ssize_t r;
    do {
        r = read(fd, buf, count);
    } while (r == -1 && errno == EINTR);
    return r;
}

static inline ssize_t safe_write(int fd, const void *buf, size_t count) {
    ssize_t w;
    size_t written = 0;
    const char *p = buf;
    while (written < count) {
        w = write(fd, p + written, count - written);
        if (w == -1) {
            if (errno == EINTR) continue;
            return -1;
        }
        written += w;
    }
    return written;
}

#endif // BETTER_UNISTD_H
