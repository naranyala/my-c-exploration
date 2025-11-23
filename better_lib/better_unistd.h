/* better_unistd.h v1.1 */
#ifndef BETTER_UNISTD_H
#define BETTER_UNISTD_H
#include <unistd.h>

static inline ssize_t bread_exact(int fd, void* buf, size_t count) {
    size_t total = 0;
    while (total < count) {
        ssize_t r = read(fd, (char*)buf + total, count - total);
        if (r <= 0) return (r == 0 && total == count) ? (ssize_t)total : -1;
        total += (size_t)r;
    }
    return (ssize_t)total;
}

static inline ssize_t bwrite_exact(int fd, const void* buf, size_t count) {
    size_t total = 0;
    while (total < count) {
        ssize_t r = write(fd, (const char*)buf + total, count - total);
        if (r <= 0) return -1;
        total += (size_t)r;
    }
    return (ssize_t)total;
}

#endif
