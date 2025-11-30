
// log.h
#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <time.h>

typedef enum { LOG_INFO, LOG_WARN, LOG_ERROR } LogLevel;

static inline void log_msg(LogLevel level, const char *msg) {
  const char *prefix = (level == LOG_INFO)   ? "[INFO]"
                       : (level == LOG_WARN) ? "[WARN]"
                                             : "[ERROR]";
  time_t now = time(NULL);
  char buf[20];
  strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&now));
  fprintf(stderr, "%s %s %s\n", buf, prefix, msg);
}

#endif
