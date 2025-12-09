// proper_time.h
#ifndef PROPER_TIME_H
#define PROPER_TIME_H

#ifdef _WIN32
#include <windows.h>
typedef double proper_time_t;
static inline proper_time_t proper_time_now() {
  LARGE_INTEGER freq, counter;
  QueryPerformanceFrequency(&freq);
  QueryPerformanceCounter(&counter);
  return (double)counter.QuadPart / (double)freq.QuadPart;
}
#else
#include <time.h>
typedef struct timespec proper_time_t;
static inline proper_time_t proper_time_now() {
  proper_time_t t;
  clock_gettime(CLOCK_MONOTONIC, &t);
  return t;
}

static inline double proper_time_diff(proper_time_t start, proper_time_t end) {
  return (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
}
#endif

#ifndef _WIN32
// For non-Windows, provide a diff helper
#define proper_time_elapsed(start) proper_time_diff(start, proper_time_now())
#else
// On Windows, proper_time_now() returns seconds directly
#define proper_time_elapsed(start) (proper_time_now() - (start))
#endif

#endif // PROPER_TIME_H
