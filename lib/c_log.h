#ifndef C_LOG_H
#define C_LOG_H

#include <stdio.h>
#include <string.h> // For basename/strrchr (optional, improves file output)
#include <time.h>

// --- Log Level Definitions ---
#define C_LOG_LEVEL_NONE 0
#define C_LOG_LEVEL_ERROR 1
#define C_LOG_LEVEL_WARN 2
#define C_LOG_LEVEL_INFO 3
#define C_LOG_LEVEL_DEBUG 4

// Set the current logging level. Default to INFO if not defined.
#ifndef C_LOG_LEVEL
#define C_LOG_LEVEL C_LOG_LEVEL_INFO
#endif

// --- Helper Functions and Macros ---

// Get the base filename (useful for POSIX/UNIX, less so for simple C, but good
// practice)
#define C_LOG_GET_FILENAME(file)                                               \
  (strrchr(file, '/')                                                          \
       ? strrchr(file, '/') + 1                                                \
       : (strrchr(file, '\\') ? strrchr(file, '\\') + 1 : file))

// Core logging function (internal)
void c_log_print(const char *level_str, const char *file, int line,
                 const char *fmt, ...);

// --- Primary API Macros (Conditional on LOG_LEVEL) ---

#if C_LOG_LEVEL >= C_LOG_LEVEL_ERROR
#define C_LOG_ERROR(fmt, ...)                                                  \
  c_log_print("ERROR", C_LOG_GET_FILENAME(__FILE__), __LINE__, fmt,            \
              ##__VA_ARGS__)
#else
#define C_LOG_ERROR(fmt, ...) (void)0
#endif

#if C_LOG_LEVEL >= C_LOG_LEVEL_WARN
#define C_LOG_WARN(fmt, ...)                                                   \
  c_log_print("WARN ", C_LOG_GET_FILENAME(__FILE__), __LINE__, fmt,            \
              ##__VA_ARGS__)
#else
#define C_LOG_WARN(fmt, ...) (void)0
#endif

#if C_LOG_LEVEL >= C_LOG_LEVEL_INFO
#define C_LOG_INFO(fmt, ...)                                                   \
  c_log_print("INFO ", C_LOG_GET_FILENAME(__FILE__), __LINE__, fmt,            \
              ##__VA_ARGS__)
#else
#define C_LOG_INFO(fmt, ...) (void)0
#endif

#if C_LOG_LEVEL >= C_LOG_LEVEL_DEBUG
#define C_LOG_DEBUG(fmt, ...)                                                  \
  c_log_print("DEBUG", C_LOG_GET_FILENAME(__FILE__), __LINE__, fmt,            \
              ##__VA_ARGS__)
#else
#define C_LOG_DEBUG(fmt, ...) (void)0
#endif

// --- Implementation Section ---
#ifdef C_LOG_IMPLEMENTATION

#include <stdarg.h>

void c_log_print(const char *level_str, const char *file, int line,
                 const char *fmt, ...) {
  time_t timer;
  char time_str[26];
  struct tm *tm_info;

  // 1. Get current time
  time(&timer);
  tm_info = localtime(&timer);
  strftime(time_str, 26, "%H:%M:%S", tm_info);

  // 2. Print header (Time [LEVEL] file:line)
  fprintf(level_str[0] == 'E' ? stderr : stdout, "[%s] %s %s:%d | ", time_str,
          level_str, file, line);

  // 3. Print message content
  va_list args;
  va_start(args, fmt);
  vfprintf(level_str[0] == 'E' ? stderr : stdout, fmt, args);
  va_end(args);

  // 4. Newline
  fprintf(level_str[0] == 'E' ? stderr : stdout, "\n");
  fflush(level_str[0] == 'E' ? stderr : stdout);
}

#endif // C_LOG_IMPLEMENTATION

#endif // C_LOG_H
