/* ergo_time.h - Ergonomic Monotonic Time */
#ifndef ERGO_TIME_H
#define ERGO_TIME_H

#include <stdint.h>

// Returns current monotonic time in milliseconds
uint64_t ergo_millis(void);

// Portable sleep
void ergo_sleep(uint32_t ms);

#endif // ERGO_TIME_H

#ifdef ERGO_IMPLEMENTATION
#include <time.h>
#include <errno.h>

// POSIX implementation (Linux/macOS)
uint64_t ergo_millis(void) {
    struct timespec ts;
    // CLOCK_MONOTONIC is not affected by system time jumps
    if (clock_gettime(CLOCK_MONOTONIC, &ts) == -1) return 0;
    return (uint64_t)(ts.tv_sec * 1000) + (uint64_t)(ts.tv_nsec / 1000000);
}

void ergo_sleep(uint32_t ms) {
    struct timespec ts;
    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000000;
    
    // Handle EINTR (interrupted by signal) automatically
    while (nanosleep(&ts, &ts) == -1 && errno == EINTR);
}
#endif
