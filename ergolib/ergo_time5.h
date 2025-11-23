/* ergo_time.h — simple time helpers */
#ifndef ERGO_TIME_H
#define ERGO_TIME_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* monotonic milliseconds since unspecified epoch (monotonic). */
int64_t ergo_now_ms(void);
/* monotonic microseconds */
int64_t ergo_now_us(void);
/* sleep milliseconds */
void ergo_sleep_ms(int ms);

#ifdef __cplusplus
}
#endif
#endif /* ERGO_TIME_H */

#ifdef ERGO_TIME_IMPLEMENTATION
#include <time.h>
#ifdef _WIN32
#include <windows.h>
int64_t ergo_now_ms(void){
    FILETIME ft; GetSystemTimeAsFileTime(&ft);
    uint64_t v = ((uint64_t)ft.dwHighDateTime << 32) | ft.dwLowDateTime;
    /* FILETIME is 100-ns since 1601; convert to ms */
    return (int64_t)(v / 10000);
}
int64_t ergo_now_us(void){
    FILETIME ft; GetSystemTimeAsFileTime(&ft);
    uint64_t v = ((uint64_t)ft.dwHighDateTime << 32) | ft.dwLowDateTime;
    return (int64_t)(v / 10);
}
void ergo_sleep_ms(int ms){ Sleep(ms); }
#else
int64_t ergo_now_ms(void){
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (int64_t)ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}
int64_t ergo_now_us(void){
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (int64_t)ts.tv_sec * 1000000 + ts.tv_nsec / 1000;
}
void ergo_sleep_ms(int ms){
    struct timespec ts;
    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000000;
    nanosleep(&ts, NULL);
}
#endif
#endif /* ERGO_TIME_IMPLEMENTATION */

