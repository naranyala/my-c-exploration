/* better_time.h */
#ifndef BETTER_TIME_H
#define BETTER_TIME_H

#include <stdint.h>

// Get monotonic time in microseconds (us)
uint64_t bt_now_us(void);
// Get monotonic time in milliseconds (ms)
uint64_t bt_now_ms(void);
// Sleep for X milliseconds (handles interrupts)
void bt_sleep_ms(int ms);

#endif // BETTER_TIME_H

/* IMPLEMENTATION */
#ifdef BETTER_TIME_IMPLEMENTATION
#define _POSIX_C_SOURCE 199309L // For clock_gettime
#include <time.h>
#include <errno.h>

uint64_t bt_now_us(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000ULL + (uint64_t)ts.tv_nsec / 1000ULL;
}

uint64_t bt_now_ms(void) {
    return bt_now_us() / 1000ULL;
}

void bt_sleep_ms(int ms) {
    struct timespec ts;
    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000000;
    
    while (nanosleep(&ts, &ts) == -1 && errno == EINTR) {
        // Loop continues if interrupted by signal, using remaining time in ts
    }
}
#endif
