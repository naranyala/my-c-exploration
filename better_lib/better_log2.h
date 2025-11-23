/* better_log.h */
#ifndef BETTER_LOG_H
#define BETTER_LOG_H

typedef enum { BL_INFO, BL_WARN, BL_ERROR, BL_DEBUG } bl_level;

// Set minimum log level (default INFO)
void bl_set_level(bl_level level);

// Main logging macro
#define log_info(...)  bl_log(BL_INFO,  __FILE__, __LINE__, __VA_ARGS__)
#define log_warn(...)  bl_log(BL_WARN,  __FILE__, __LINE__, __VA_ARGS__)
#define log_err(...)   bl_log(BL_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define log_debug(...) bl_log(BL_DEBUG, __FILE__, __LINE__, __VA_ARGS__)

void bl_log(bl_level level, const char* file, int line, const char* fmt, ...);

#endif

/* IMPLEMENTATION */
#ifdef BETTER_LOG_IMPLEMENTATION
#include <stdio.h>
#include <stdarg.h>
#include <time.h>

static bl_level g_min_level = BL_INFO;

void bl_set_level(bl_level level) { g_min_level = level; }

void bl_log(bl_level level, const char* file, int line, const char* fmt, ...) {
    if (level > g_min_level && level != BL_ERROR) return; // Simplified filtering

    // ANSI Colors
    const char* color = "\x1b[0m";
    const char* label = "[INFO]";
    if (level == BL_WARN)  { color = "\x1b[33m"; label = "[WARN]"; }
    if (level == BL_ERROR) { color = "\x1b[31m"; label = "[ERR ]"; }
    if (level == BL_DEBUG) { color = "\x1b[36m"; label = "[DBUG]"; }
    const char* reset = "\x1b[0m";

    // Time
    time_t t = time(NULL);
    struct tm* tm_info = localtime(&t);
    char time_buf[9];
    strftime(time_buf, sizeof(time_buf), "%H:%M:%S", tm_info);

    // Print prefix
    fprintf(stderr, "%s %s%s%s %s:%d: ", time_buf, color, label, reset, file, line);

    // Print user message
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);

    fprintf(stderr, "\n");
}
#endif
