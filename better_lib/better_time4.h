
#ifndef BETTER_TIME_H
#define BETTER_TIME_H

#include <time.h>
#include <stdio.h>

// Format current time as ISO8601
static inline void time_to_iso8601(char *buf, size_t buf_size) {
    time_t t = time(NULL);
    struct tm tm;
    localtime_r(&t, &tm);
    strftime(buf, buf_size, "%Y-%m-%dT%H:%M:%S%z", &tm);
}

// Simple stopwatch
typedef struct {
    struct timespec start;
} stopwatch_t;

static inline void stopwatch_start(stopwatch_t *sw) {
    clock_gettime(CLOCK_MONOTONIC, &sw->start);
}

static inline double stopwatch_elapsed(stopwatch_t *sw) {
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    return (now.tv_sec - sw->start.tv_sec) +
           (now.tv_nsec - sw->start.tv_nsec) / 1e9;
}

#endif // BETTER_TIME_H
