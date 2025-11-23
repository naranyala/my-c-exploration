/* better_fcntl.h v1.0 */
#ifndef BETTER_FCNTL_H
#define BETTER_FCNTL_H
#include <fcntl.h>
#include <unistd.h>

static inline int bopen(const char* path, int flags, ...) {
    mode_t mode = 0;
    if (flags & O_CREAT) {
        va_list ap;
        va_start(ap, flags);
        mode = va_arg(ap, mode_t);
        va_end(ap);
    }
    int fd = open(path, flags, mode);
    if (fd < 0) { perror(path); abort(); }
    return fd;
}

static inline void bfd_set_nonblocking(int fd, int enable) {
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, enable ? (flags | O_NONBLOCK) : (flags & ~O_NONBLOCK));
}

#endif
