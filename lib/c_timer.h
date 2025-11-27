#ifndef C_TIMER_H
#define C_TIMER_H

#include <stdint.h>
#include <time.h>

// Handle platform differences
#if defined(_WIN32)
#include <windows.h>
typedef LARGE_INTEGER c_timer_handle_t;
#elif defined(__APPLE__) || defined(__linux__) || defined(__unix__)
typedef struct timespec c_timer_handle_t;
#else
// Fallback to standard clock_t if platform is unknown
typedef clock_t c_timer_handle_t;
#endif

// --- Structure and Functions ---

typedef struct {
    c_timer_handle_t start_time;
#if defined(_WIN32)
    LONGLONG frequency; // Frequency needed for Windows calculation
#endif
} c_timer_t;

// Function declarations
void c_timer_start(c_timer_t *timer);
double c_timer_stop(c_timer_t *timer);

// --- Implementation Section ---
#ifdef C_TIMER_IMPLEMENTATION

#if defined(_WIN32)

void c_timer_start(c_timer_t *timer) {
    QueryPerformanceFrequency((LARGE_INTEGER*)&timer->frequency);
    QueryPerformanceCounter(&timer->start_time);
}

double c_timer_stop(c_timer_t *timer) {
    LARGE_INTEGER end_time;
    QueryPerformanceCounter(&end_time);

    // Calculate difference in seconds
    return (double)(end_time.QuadPart - timer->start_time.QuadPart) / (double)timer->frequency;
}

#elif defined(__APPLE__) || defined(__linux__) || defined(__unix__)

void c_timer_start(c_timer_t *timer) {
    // Use monotonic clock for reliable duration measurement
    clock_gettime(CLOCK_MONOTONIC, timer->start_time);
}

double c_timer_stop(c_timer_t *timer) {
    struct timespec end_time;
    clock_gettime(CLOCK_MONOTONIC, &end_time);

    long seconds = end_time.tv_sec - timer->start_time.tv_sec;
    long nanoseconds = end_time.tv_nsec - timer->start_time.tv_nsec;

    // Convert total time to seconds
    return (double)seconds + (double)nanoseconds / 1000000000.0;
}

#else 

// Fallback implementation using standard C clock()
void c_timer_start(c_timer_t *timer) {
    timer->start_time = clock();
}

double c_timer_stop(c_timer_t *timer) {
    return (double)(clock() - timer->start_time) / CLOCKS_PER_SEC;
}

#endif // Platform implementations

#endif // C_TIMER_IMPLEMENTATION

#endif // C_TIMER_H
