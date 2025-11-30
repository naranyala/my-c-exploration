#ifdef _WIN32
#include <windows.h>
typedef LARGE_INTEGER timer_t;
static void timer_start(timer_t *t) { QueryPerformanceCounter(t); }
static double timer_elapsed(timer_t start) {
  LARGE_INTEGER freq, end;
  QueryPerformanceCounter(&end);
  QueryPerformanceFrequency(&freq);
  return (double)(end.QuadPart - start.QuadPart) / freq.QuadPart;
}
#else
#include <time.h>
typedef struct timespec timer_t;
static void timer_start(timer_t *t) { clock_gettime(CLOCK_MONOTONIC, t); }
static double timer_elapsed(timer_t start) {
  struct timespec end;
  clock_gettime(CLOCK_MONOTONIC, &end);
  return (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
}
#endif
