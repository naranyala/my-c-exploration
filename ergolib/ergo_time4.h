/* ergo_time.h */
#pragma once
#include <time.h>
#include <stdio.h>

static inline double now_monotonic_sec(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

static inline void sleep_ms(unsigned ms) {
    struct timespec ts = { ms / 1000, (ms % 1000) * 1000000 };
    nanosleep(&ts, NULL);
}

static inline char* duration_str(double seconds, char buf[32]) {
    if (seconds < 1) sprintf(buf, "%.1fms", seconds*1000);
    else if (seconds < 60) sprintf(buf, "%.1fs", seconds);
    else if (seconds < 3600) sprintf(buf, "%.1fm", seconds/60);
    else if (seconds < 86400) sprintf(buf, "%.1fh", seconds/3600);
    else sprintf(buf, "%.1fd", seconds/86400);
    return buf;
}

