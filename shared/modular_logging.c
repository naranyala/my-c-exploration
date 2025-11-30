#include <stdio.h>
#include <time.h>

#define LOG_ERROR 0
#define LOG_WARN 1
#define LOG_INFO 2
#define LOG_DEBUG 3

#ifndef LOG_LEVEL
#define LOG_LEVEL LOG_INFO
#endif

#define log_log(level, fmt, ...)                                               \
  do {                                                                         \
    if (level > LOG_LEVEL)                                                     \
      break;                                                                   \
    char *lvl_str = (level == LOG_ERROR)  ? "ERROR"                            \
                    : (level == LOG_WARN) ? "WARN"                             \
                    : (level == LOG_INFO) ? "INFO"                             \
                                          : "DEBUG";                           \
    time_t t = time(NULL);                                                     \
    char timestr[20];                                                          \
    strftime(timestr, sizeof(timestr), "%Y-%m-%d %H:%M:%S", localtime(&t));    \
    fprintf(stderr, "\33[1;3%dm[%s] %s: " fmt "\33[0m\n",                      \
            level == LOG_ERROR  ? 1                                            \
            : level == LOG_WARN ? 3                                            \
                                : 2,                                           \
            timestr, lvl_str, ##__VA_ARGS__);                                  \
  } while (0)

// log_log(LOG_INFO, "Processing file %s", filename);
// log_log(LOG_DEBUG, "Pointer = %p, value = %d", ptr, value);
