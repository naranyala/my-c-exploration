/* ergo_log.h */
#pragma once
#include <stdio.h>
#include <time.h>
#include <string.h>

#define LOG_TRACE(...) ergo_log("TRACE", "\x1b[36m", __VA_ARGS__)  // cyan
#define LOG_DEBUG(...) ergo_log("DEBUG", "\x1b[34m", __VA_ARGS__)  // blue
#define LOG_INFO(...)  ergo_log("INFO ", "\x1b[32m", __VA_ARGS__)  // green
#define LOG_WARN(...)  ergo_log("WARN ", "\x1b[33m", __VA_ARGS__)  // yellow
#define LOG_ERROR(...) ergo_log("ERROR", "\x1b[31m", __VA_ARGS__)  // red
#define LOG_FATAL(...) ergo_log("FATAL", "\x1b[35m", __VA_ARGS__), exit(1)

static inline void ergo_log(const char *level, const char *color, const char *fmt, ...) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    struct tm tm;
    localtime_r(&ts.tv_sec, &tm);
    char timebuf[32];
    strftime(timebuf, sizeof(timebuf), "%H:%M:%S", &tm);

    FILE *out = strcmp(level, "ERROR") >= 0 && strcmp(level, "FATAL") != 0 ? stderr : stdout;
    fprintf(out, "%s%s.%03ld \x1b[1m%s\x1b[0m %s:", timebuf, (int)(ts.tv_nsec/1000000), level, color);

    va_list va;
    va_start(va, fmt);
    vfprintf(out, fmt, va);
    va_end(va);
    fprintf(out, "\x1b[0m\n");
}

