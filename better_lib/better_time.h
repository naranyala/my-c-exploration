
/* better_time.h */

#ifndef BETTER_TIME_H
#define BETTER_TIME_H

#include <time.h>

#ifdef _WIN32
#include <windows.h>
static inline void bsleep_ms(int ms) {
    Sleep(ms);
}
#else
#include <unistd.h>
static inline void bsleep_ms(int ms) {
    usleep(ms * 1000);
}
#endif

typedef struct {
    struct timespec start;
} bstopwatch;

static inline void btimer_start(bstopwatch *t) {
    clock_gettime(CLOCK_MONOTONIC, &t->start);
}

static inline double btimer_elapsed_ms(bstopwatch *t) {
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    long sec  = now.tv_sec  - t->start.tv_sec;
    long nsec = now.tv_nsec - t->start.tv_nsec;
    return (double)sec * 1000.0 + (double)nsec / 1e6;
}

#endif
