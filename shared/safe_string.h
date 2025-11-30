
// strutil.h
#ifndef STRUTIL_H
#define STRUTIL_H

#include <string.h>

static inline int str_eq(const char *a, const char *b) {
  return strcmp(a, b) == 0;
}

static inline int str_starts(const char *s, const char *prefix) {
  return strncmp(s, prefix, strlen(prefix)) == 0;
}

#endif
