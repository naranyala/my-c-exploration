/* ergo_time.h
 * Simple time functions: current time in sec/ms, sleep, format.
 */

#ifndef ERGO_TIME_H
#define ERGO_TIME_H

#include <stdint.h>  /* uint64_t */

#ifdef __cplusplus
extern "C" {
#endif

/* Returns seconds since Unix epoch */
double ergo_time_now(void);

/* Returns milliseconds since Unix epoch */
uint64_t ergo_time_now_ms(void);

/* Sleep for milliseconds (cross-platform compatible in practice) */
void ergo_sleep_ms(unsigned int ms);

#ifdef __cplusplus
}
#endif

#endif /* ERGO_TIME_H */

#ifdef ERGO_TIME_IMPLEMENTATION

#include <time.h>

#if defined(_WIN32) || defined(_WIN64)
  #include <windows.h>
#else
  #include <unistd.h>  /* usleep */
#endif

double ergo_time_now(void) {
    return (double)time(NULL);
}

uint64_t ergo_time_now_ms(void) {
#if defined(_WIN32) || defined(_WIN64)
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);
    uint64_t t = ((uint64_t)ft.dwHighDateTime << 32) | ft.dwLowDateTime;
    return (t - 116444736000000000ULL) / 10000;  // convert to ms since 1970
#else
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (uint64_t)ts.tv_sec * 1000 + (uint64_t)ts.tv_nsec / 1000000;
#endif
}

void ergo_sleep_ms(unsigned int ms) {
#if defined(_WIN32) || defined(_WIN64)
    Sleep(ms);
#else
    usleep(ms * 1000);
#endif
}

#endif /* ERGO_TIME_IMPLEMENTATION */
