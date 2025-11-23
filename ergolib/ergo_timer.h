#ifndef ERGO_TIMER_H
#define ERGO_TIMER_H

#include <time.h>
#include <stdio.h>

typedef struct {
    struct timespec start;
} ergo_timer;

static inline ergo_timer ergo_timer_start(void) {
    ergo_timer t;
    clock_gettime(CLOCK_MONOTONIC, &t.start);
    return t;
}

static inline double ergo_timer_elapsed_ms(ergo_timer t) {
    struct timespec end;
    clock_gettime(CLOCK_MONOTONIC, &end);
    return (end.tv_sec - t.start.tv_sec) * 1000.0 +
           (end.tv_nsec - t.start.tv_nsec) / 1e6;
}

#endif // ERGO_TIMER_H

