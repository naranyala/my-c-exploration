/* better_time.h v1.3 - Monotonic clocks, timespec math, sleep helpers */
#ifndef BETTER_TIME_H
#define BETTER_TIME_H
#include <time.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Monotonic double seconds since program start */
static inline double bt_monotonic_sec(void) {
    struct timespec ts;
#if defined(CLOCK_MONOTONIC)
    clock_gettime(CLOCK_MONOTONIC, &ts);
#elif defined(CLOCK_UPTIME_RAW)
    clock_gettime(CLOCK_UPTIME_RAW, &ts);
#else
    clock_gettime(CLOCK_REALTIME, &ts);
#endif
    return (double)ts.tv_sec + (double)ts.tv_nsec * 1e-9;
}

/* High-res sleep (supports sub-millisecond) */
static inline void bt_sleep(double seconds) {
    if (seconds <= 0) return;
    struct timespec ts = {
        .tv_sec  = (time_t)seconds,
        .tv_nsec = (long)((seconds - (time_t)seconds) * 1e9)
    };
    while (nanosleep(&ts, &ts) == -1 && errno == EINTR);
}

/* timespec arithmetic */
static inline struct timespec bt_add(struct timespec a, struct timespec b) {
    struct timespec r;
    r.tv_sec  = a.tv_sec + b.tv_sec;
    r.tv_nsec = a.tv_nsec + b.tv_nsec;
    if (r.tv_nsec >= 1000000000L) { r.tv_sec++; r.tv_nsec -= 1000000000L; }
    return r;
}

static inline struct timespec bt_sub(struct timespec a, struct timespec b) {
    struct timespec r;
    r.tv_sec  = a.tv_sec - b.tv_sec;
    r.tv_nsec = a.tv_nsec - b.tv_nsec;
    if (r.tv_nsec < 0) { r.tv_sec--; r.tv_nsec += 1000000000L; }
    return r;
}

static inline double bt_to_double(struct timespec ts) {
    return (double)ts.tv_sec + (double)ts.tv_nsec * 1e-9;
}

#ifdef __cplusplus
}
#endif
#endif /* BETTER_TIME_H */
