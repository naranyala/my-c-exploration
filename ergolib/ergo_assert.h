/* ergo_assert.h
 * Enhanced assertions and panic macros with messages.
 */

#ifndef ERGO_ASSERT_H
#define ERGO_ASSERT_H

#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Panic with formatted message and abort */
void ergo_panic(const char *fmt, ...);

#ifdef __cplusplus
}
#endif

/* Disable assertions in release */
#if defined(ERGO_NDEBUG) || defined(NDEBUG)
  #define ergo_assert(cond, ...) ((void)0)
#else
  #include <assert.h>
  /* Note: uses assert internally for debugger support */
  #define ergo_assert(cond, ...) \
    do { \
      if (!(cond)) { \
        fprintf(stderr, "Assertion failed: %s (%s:%d)\n", #cond, __FILE__, __LINE__); \
        ergo_panic(__VA_ARGS__); \
      } \
    } while(0)
#endif

#endif /* ERGO_ASSERT_H */

#ifdef ERGO_ASSERT_IMPLEMENTATION

#include <stdarg.h>

void ergo_panic(const char *fmt, ...) {
    if (fmt) {
        va_list args;
        va_start(args, fmt);
        vfprintf(stderr, fmt, args);
        va_end(args);
        fputc('\n', stderr);
    } else {
        fputs("Panic!\n", stderr);
    }
    abort();
}

#endif /* ERGO_ASSERT_IMPLEMENTATION */
