// proper_log.h
#ifndef PROPER_LOG_H
#define PROPER_LOG_H

#include <stdio.h>
#include <time.h>

#define PROPER_LOG_LEVEL_DEBUG 0
#define PROPER_LOG_LEVEL_INFO 1
#define PROPER_LOG_LEVEL_WARN 2
#define PROPER_LOG_LEVEL_ERROR 3
#define PROPER_LOG_LEVEL_NONE 4

#ifndef PROPER_LOG_MIN_LEVEL
#define PROPER_LOG_MIN_LEVEL PROPER_LOG_LEVEL_DEBUG
#endif

static inline void proper_log_write(int level, const char *file, int line,
                                    const char *fmt, ...) {
  if (level < PROPER_LOG_MIN_LEVEL)
    return;

  const char *level_str[] = {"DEBUG", "INFO ", "WARN ", "ERROR"};
  char time_buf[32];
  time_t t = time(NULL);
  struct tm *tm = localtime(&t);
  strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S", tm);

  fprintf(stderr, "[%s] %s %s:%d: ", time_buf, level_str[level], file, line);

  va_list args;
  va_start(args, fmt);
  vfprintf(stderr, fmt, args);
  va_end(args);
  fputc('\n', stderr);
}

#define proper_log_debug(fmt, ...)                                             \
  proper_log_write(PROPER_LOG_LEVEL_DEBUG, __FILE__, __LINE__, fmt,            \
                   ##__VA_ARGS__)
#define proper_log_info(fmt, ...)                                              \
  proper_log_write(PROPER_LOG_LEVEL_INFO, __FILE__, __LINE__, fmt,             \
                   ##__VA_ARGS__)
#define proper_log_warn(fmt, ...)                                              \
  proper_log_write(PROPER_LOG_LEVEL_WARN, __FILE__, __LINE__, fmt,             \
                   ##__VA_ARGS__)
#define proper_log_error(fmt, ...)                                             \
  proper_log_write(PROPER_LOG_LEVEL_ERROR, __FILE__, __LINE__, fmt,            \
                   ##__VA_ARGS__)

#endif // PROPER_LOG_H
