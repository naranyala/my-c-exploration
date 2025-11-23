/* ergo_rand.h — secure random byte & integer helpers */
#ifndef ERGO_RAND_H
#define ERGO_RAND_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Fills buf[size] with cryptographically strong random bytes.
   Returns 0 on success, -1 on failure. */
int ergo_rand_bytes(void *buf, size_t size);

/* Returns random 64-bit integer (secure). */
uint64_t ergo_rand_u64(void);

/* Returns random integer in [min, max] inclusive. */
int64_t ergo_rand_range(int64_t min, int64_t max);

#ifdef __cplusplus
}
#endif
#endif /* ERGO_RAND_H */


#ifdef ERGO_RAND_IMPLEMENTATION
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#include <bcrypt.h>
#pragma comment(lib, "bcrypt.lib")
int ergo_rand_bytes(void *buf, size_t size){
    return BCryptGenRandom(NULL, buf, (ULONG)size, BCRYPT_USE_SYSTEM_PREFERRED_RNG) == 0 ? 0 : -1;
}
#else
#include <fcntl.h>
#include <unistd.h>
int ergo_rand_bytes(void *buf, size_t size){
    int fd = open("/dev/urandom", O_RDONLY);
    if (fd < 0) return -1;
    ssize_t n = read(fd, buf, size);
    close(fd);
    return (n == (ssize_t)size) ? 0 : -1;
}
#endif

uint64_t ergo_rand_u64(void){
    uint64_t r;
    if (ergo_rand_bytes(&r, sizeof(r)) != 0) return 0;
    return r;
}

int64_t ergo_rand_range(int64_t min, int64_t max){
    if (max < min) { int64_t t=min; min=max; max=t; }
    uint64_t r = ergo_rand_u64();
    uint64_t span = (uint64_t)(max - min + 1);
    return (int64_t)(min + (r % span));
}
#endif /* ERGO_RAND_IMPLEMENTATION */

