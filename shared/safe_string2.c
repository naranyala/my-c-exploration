#include <stdio.h>
#include <string.h>

int snprintfcat(char *dst, size_t size, const char *fmt, ...) {
  size_t used = strlen(dst);
  if (size <= used)
    return -1;
  va_list ap;
  va_start(ap, fmt);
  int n = vsnprintf(dst + used, size - used, fmt, ap);
  va_end(ap);
  return n;
}

// strdup with malloc check
char *xstrdup(const char *s) {
  char *p = strdup(s);
  if (!p)
    abort();
  return p;
}
