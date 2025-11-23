/* ergo_fd.h — POSIX-style fd helpers */
#ifndef ERGO_FD_H
#define ERGO_FD_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Make fd nonblocking. Returns 0 on success. */
int ergo_fd_set_nonblock(int fd);

/* Make fd blocking. */
int ergo_fd_set_block(int fd);

/* Read exactly size bytes or fail; returns 0 or -1. */
int ergo_fd_read_full(int fd, void *buf, size_t size);

/* Write all bytes; returns 0 or -1. */
int ergo_fd_write_full(int fd, const void *buf, size_t size);

#ifdef __cplusplus
}
#endif
#endif /* ERGO_FD_H */

#ifdef ERGO_FD_IMPLEMENTATION
#ifdef _WIN32
#error "Windows not implemented"
#else
#include <unistd.h>
#include <fcntl.h>

int ergo_fd_set_nonblock(int fd){
    int fl = fcntl(fd, F_GETFL, 0);
    if (fl < 0) return -1;
    return fcntl(fd, F_SETFL, fl | O_NONBLOCK);
}
int ergo_fd_set_block(int fd){
    int fl = fcntl(fd, F_GETFL, 0);
    if (fl < 0) return -1;
    return fcntl(fd, F_SETFL, fl & ~O_NONBLOCK);
}

int ergo_fd_read_full(int fd, void *buf, size_t size){
    size_t done = 0;
    while (done < size){
        ssize_t n = read(fd, (char*)buf + done, size - done);
        if (n <= 0) return -1;
        done += (size_t)n;
    }
    return 0;
}
int ergo_fd_write_full(int fd, const void *buf, size_t size){
    size_t done = 0;
    while (done < size){
        ssize_t n = write(fd, (char*)buf + done, size - done);
        if (n <= 0) return -1;
        done += (size_t)n;
    }
    return 0;
}
#endif
#endif /* ERGO_FD_IMPLEMENTATION */

