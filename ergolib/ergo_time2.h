#ifndef ERGO_TIME_H
#define ERGO_TIME_H

#include <time.h>
#include <stdio.h>

// Sleep in milliseconds
static inline void ergo_sleep_ms(long ms) {
    struct timespec ts;
    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000000;
    nanosleep(&ts, NULL);
}

// ISO8601 timestamp string
static inline void ergo_time_iso8601(char *buf, size_t len) {
    time_t t = time(NULL);
    struct tm tm;
    gmtime_r(&t, &tm);
    strftime(buf, len, "%Y-%m-%dT%H:%M:%SZ", &tm);
}

#endif // ERGO_TIME_H

