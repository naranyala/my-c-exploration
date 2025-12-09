// proper_utf8.h
#ifndef PROPER_UTF8_H
#define PROPER_UTF8_H

#include <stddef.h>
#include <stdint.h>

// Returns number of UTF-8 codepoints, or -1 if invalid
static inline int proper_utf8_codepoint_count(const char *s) {
  if (!s)
    return -1;
  int count = 0;
  const uint8_t *p = (const uint8_t *)s;
  while (*p) {
    if (*p <= 0x7F) {
      p++;
    } else if ((*p & 0xE0) == 0xC0) {
      if ((p[1] & 0xC0) != 0x80)
        return -1;
      p += 2;
    } else if ((*p & 0xF0) == 0xE0) {
      if ((p[1] & 0xC0) != 0x80 || (p[2] & 0xC0) != 0x80)
        return -1;
      p += 3;
    } else if ((*p & 0xF8) == 0xF0) {
      if ((p[1] & 0xC0) != 0x80 || (p[2] & 0xC0) != 0x80 ||
          (p[3] & 0xC0) != 0x80)
        return -1;
      p += 4;
    } else {
      return -1; // invalid lead byte
    }
    count++;
  }
  return count;
}

// Return byte length of next UTF-8 character, or 0 if invalid
static inline size_t proper_utf8_char_bytes(const char *s) {
  if (!s)
    return 0;
  uint8_t c = *(const uint8_t *)s;
  if (c <= 0x7F)
    return 1;
  if ((c & 0xE0) == 0xC0)
    return 2;
  if ((c & 0xF0) == 0xE0)
    return 3;
  if ((c & 0xF8) == 0xF0)
    return 4;
  return 0;
}

#endif // PROPER_UTF8_H
