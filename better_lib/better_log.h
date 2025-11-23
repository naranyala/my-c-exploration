
/* better_log.h */

#ifndef BETTER_LOG_H
#define BETTER_LOG_H

#include <stdio.h>
#include <stdarg.h>
#include <time.h>

typedef enum {
    BLOG_TRACE = 0,
    BLOG_INFO  = 1,
    BLOG_WARN  = 2,
    BLOG_ERR   = 3
} blog_level;

#ifndef BLOG_MIN_LEVEL
#define BLOG_MIN_LEVEL BLOG_TRACE
#endif

static inline void blog_log(blog_level lvl,
                            const char *fmt, ...) {
    if (lvl < BLOG_MIN_LEVEL) return;

    const char *names[] = {"TRACE", "INFO", "WARN", "ERR"};
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);

    fprintf(stderr, "[%02d:%02d:%02d] %s: ",
            tm->tm_hour, tm->tm_min, tm->tm_sec,
            names[lvl]);

    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);

    fprintf(stderr, "\n");
}

#endif
