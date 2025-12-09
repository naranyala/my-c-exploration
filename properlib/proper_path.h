// proper_path.h
#ifndef PROPER_PATH_H
#define PROPER_PATH_H

#include <stddef.h>
#include <string.h>

#ifdef _WIN32
#define PROPER_PATH_SEP '\\'
#define PROPER_PATH_SEP_STR "\\"
#else
#define PROPER_PATH_SEP '/'
#define PROPER_PATH_SEP_STR "/"
#endif

// Return pointer to last separator in path, or NULL
static inline const char *proper_path_basename(const char *path) {
  if (!path)
    return NULL;
  const char *p = path;
  const char *last = p;
  while (*p) {
    if (*p == '/' || *p == '\\')
      last = p + 1;
    p++;
  }
  return last;
}

// Copy dirname (everything before last separator) into out (must be >=
// strlen(path)) Returns length written (excluding null terminator)
static inline size_t proper_path_dirname(const char *path, char *out,
                                         size_t out_size) {
  if (!path || !out || out_size == 0)
    return 0;
  size_t len = strlen(path);
  if (len == 0) {
    out[0] = '.';
    out[1] = '\0';
    return 1;
  }
  const char *last_slash = NULL;
  for (size_t i = 0; i < len; ++i) {
    if (path[i] == '/' || path[i] == '\\')
      last_slash = &path[i];
  }
  if (!last_slash) {
    out[0] = '.';
    out[1] = '\0';
    return 1;
  }
  size_t to_copy = (size_t)(last_slash - path);
  if (to_copy >= out_size)
    to_copy = out_size - 1;
  memcpy(out, path, to_copy);
  out[to_copy] = '\0';
  return to_copy;
}

// Join two path components with correct separator
// Writes result to out (must be large enough)
static inline void proper_path_join(const char *a, const char *b, char *out,
                                    size_t out_size) {
  if (!a || !b || !out || out_size == 0) {
    if (out)
      out[0] = '\0';
    return;
  }
  size_t len_a = strlen(a);
  size_t len_b = strlen(b);
  if (len_a == 0) {
    proper_path_join(".", b, out, out_size);
    return;
  }
  if (len_b == 0) {
    proper_path_join(a, ".", out, out_size);
    return;
  }
  // Trim trailing separator from a
  while (len_a > 0 && (a[len_a - 1] == '/' || a[len_a - 1] == '\\'))
    len_a--;
  // Trim leading separator from b
  while (*b == '/' || *b == '\\') {
    b++;
    len_b--;
  }
  if (len_a + 1 + len_b + 1 > out_size) {
    out[0] = '\0';
    return;
  }
  memcpy(out, a, len_a);
  out[len_a] = PROPER_PATH_SEP;
  memcpy(out + len_a + 1, b, len_b);
  out[len_a + 1 + len_b] = '\0';
}

#endif // PROPER_PATH_H
