/* ergo_log.h - Colorful, Context-Aware Logging */
#ifndef ERGO_LOG_H
#define ERGO_LOG_H

// API Macros
#define ergo_info(...)  ergo_log_impl("INFO",  "\x1b[32m", __FILE__, __LINE__, __VA_ARGS__)
#define ergo_warn(...)  ergo_log_impl("WARN",  "\x1b[33m", __FILE__, __LINE__, __VA_ARGS__)
#define ergo_error(...) ergo_log_impl("ERROR", "\x1b[31m", __FILE__, __LINE__, __VA_ARGS__)

// Internal function (don't call directly)
void ergo_log_impl(const char* level, const char* color, const char* file, int line, const char* fmt, ...);

#endif // ERGO_LOG_H

#ifdef ERGO_IMPLEMENTATION
#include <stdio.h>
#include <stdarg.h>
#include <time.h>

void ergo_log_impl(const char* level, const char* color, const char* file, int line, const char* fmt, ...) {
    // 1. Get Timestamp
    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    char time_buf[9];
    strftime(time_buf, sizeof(time_buf), "%H:%M:%S", t);

    // 2. Print Prefix: [TIME] [LEVEL] file:line
    // \x1b[0m resets color
    fprintf(stderr, "%s [%s%s\x1b[0m] \x1b[90m%s:%d:\x1b[0m ", 
        time_buf, color, level, file, line);

    // 3. Print User Message
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);

    // 4. Newline
    fprintf(stderr, "\n");
}
#endif
