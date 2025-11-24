
#ifndef OSRAND_H
#define OSRAND_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

int osrand_bytes(void *buf, size_t n);

#ifdef __cplusplus
}
#endif

#ifdef OSRAND_IMPLEMENTATION

#if defined(_WIN32)
#include <windows.h>
#include <bcrypt.h>
#pragma comment(lib, "bcrypt.lib")

int osrand_bytes(void *buf, size_t n) {
    return BCryptGenRandom(NULL, buf, (ULONG)n, BCRYPT_USE_SYSTEM_PREFERRED_RNG) == 0;
}

#elif defined(__linux__)
#include <sys/random.h>
int osrand_bytes(void *buf, size_t n) {
    return getrandom(buf, n, 0) == (ssize_t)n;
}

#else
#include <fcntl.h>
#include <unistd.h>
int osrand_bytes(void *buf, size_t n) {
    int fd = open("/dev/urandom", O_RDONLY);
    if (fd < 0) return 0;
    ssize_t r = read(fd, buf, n);
    close(fd);
    return r == (ssize_t)n;
}

#endif
#endif // OSRAND_IMPLEMENTATION

#endif // OSRAND_H
