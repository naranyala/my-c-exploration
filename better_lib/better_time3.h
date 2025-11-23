// better_time.h
#ifndef BETTER_TIME_H
#define BETTER_TIME_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Monotonic time in nanoseconds (best effort)
uint64_t b_monotime_ns(void);

// Sleep for milliseconds (portable)
void b_sleep_ms(uint64_t ms);

// Format current local time as ISO8601 string (e.g., "2025-11-23T14:30:45")
// Buffer must be at least 20 bytes.
void b_time_iso8601(char* buf, size_t len);

#ifdef __cplusplus
}
#endif

#ifdef BETTER_TIME_IMPL

#if defined(_WIN32) || defined(_WIN64)
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    #include <time.h>
#else
    #include <time.h>
    #include <unistd.h>
#endif

#include <string.h>
#include <stdio.h>

uint64_t b_monotime_ns(void) {
#if defined(_WIN32) || defined(_WIN64)
    static LARGE_INTEGER freq;
    static int init = 0;
    if (!init) {
        QueryPerformanceFrequency(&freq);
        init = 1;
    }
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    return (uint64_t)((now.QuadPart * 1000000000LL) / freq.QuadPart);
#else
    struct timespec ts;
#ifdef CLOCK_MONOTONIC
    clock_gettime(CLOCK_MONOTONIC, &ts);
#else
    clock_gettime(CLOCK_REALTIME, &ts); // fallback
#endif
    return (uint64_t)ts.tv_sec * 1000000000ULL + (uint64_t)ts.tv_nsec;
#endif
}

void b_sleep_ms(uint64_t ms) {
    if (ms == 0) return;
#if defined(_WIN32) || defined(_WIN64)
    Sleep((DWORD)ms);
#else
    struct timespec ts = {
        .tv_sec = (time_t)(ms / 1000),
        .tv_nsec = (long)((ms % 1000) * 1000000)
    };
    nanosleep(&ts, NULL);
#endif
}

void b_time_iso8601(char* buf, size_t len) {
    if (!buf || len < 20) return;
    time_t t = time(NULL);
    struct tm tm;
#ifdef _WIN32
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif
    strftime(buf, len, "%Y-%m-%dT%H:%M:%S", &tm);
}

#endif // BETTER_TIME_IMPL

#endif // BETTER_TIME_H
