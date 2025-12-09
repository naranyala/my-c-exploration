// proper_str.h
#ifndef PROPER_STR_H
#define PROPER_STR_H

#include <stddef.h>
#include <string.h>

// Safe string copy with guaranteed null termination
// Returns number of chars copied (excluding null terminator)
static inline size_t proper_strcpy_safe(char *dest, const char *src,
                                        size_t dest_size) {
  if (!dest || !src || dest_size == 0)
    return 0;
  size_t len = strlen(src);
  size_t to_copy = (len >= dest_size) ? dest_size - 1 : len;
  memcpy(dest, src, to_copy);
  dest[to_copy] = '\0';
  return to_copy;
}

// Trim whitespace from both ends (in-place)
static inline char *proper_strtrim(char *str) {
  if (!str)
    return NULL;
  char *end;
  while (*str == ' ' || *str == '\t' || *str == '\n' || *str == '\r')
    str++;
  if (*str == '\0')
    return str;
  end = str + strlen(str) - 1;
  while (end > str &&
         (*end == ' ' || *end == '\t' || *end == '\n' || *end == '\r'))
    end--;
  *(end + 1) = '\0';
  return str;
}

#endif // PROPER_STR_H
