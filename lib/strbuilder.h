#ifndef STRBUILDER_H
#define STRBUILDER_H

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
   ╔══════════════════════════════════════════════════════════╗
   ║              Ultimate StrBuilder v1.0 (2025)             ║
   ║  Zero-allocation failures · Blazing fast · No surprises ║
   ╚══════════════════════════════════════════════════════════╝
*/

typedef struct {
  char *data;      // null-terminated string
  size_t capacity; // total allocated (including null terminator)
  size_t len;      // length without null terminator
} strbuilder_t;

/* Create new builder */
static inline strbuilder_t strbuilder_new(const char *initial) {
  strbuilder_t sb = {0};
  if (!initial || !*initial)
    return sb;

  sb.len = strlen(initial);
  sb.capacity = sb.len + 1;
  sb.data = (char *)malloc(sb.capacity);
  if (sb.data)
    memcpy(sb.data, initial, sb.capacity);
  return sb;
}

/* Free the builder */
static inline void strbuilder_free(strbuilder_t *sb) {
  if (!sb)
    return;
  free(sb->data);
  sb->data = NULL;
  sb->capacity = sb->len = 0;
}

/* Ensure there's room for at least `need` more bytes + null terminator */
static inline int strbuilder_ensure(strbuilder_t *sb, size_t need) {
  size_t required = sb->len + need + 1;
  if (required <= sb->capacity)
    return 1;

  size_t new_cap = sb->capacity ? sb->capacity * 2 : 64;
  while (new_cap < required)
    new_cap *= 2;

  char *new_data = (char *)realloc(sb->data, new_cap);
  if (!new_data)
    return 0;

  sb->data = new_data;
  sb->capacity = new_cap;
  return 1;
}

/* Append raw C string */
static inline int strbuilder_append(strbuilder_t *sb, const char *str) {
  if (!str)
    return 1;
  size_t add = strlen(str);
  if (!strbuilder_ensure(sb, add))
    return 0;
  memcpy(sb->data + sb->len, str, add);
  sb->len += add;
  sb->data[sb->len] = '\0';
  return 1;
}

/* Append single char */
static inline int strbuilder_append_char(strbuilder_t *sb, char c) {
  if (!strbuilder_ensure(sb, 1))
    return 0;
  sb->data[sb->len++] = c;
  sb->data[sb->len] = '\0';
  return 1;
}

/* printf-style append */
static inline int strbuilder_appendf(strbuilder_t *sb, const char *fmt, ...) {
  va_list args, args_copy;
  va_start(args, fmt);
  va_copy(args_copy, args);

  int needed = vsnprintf(NULL, 0, fmt, args_copy);
  va_end(args_copy);
  if (needed < 0) {
    va_end(args);
    return 0;
  }

  if (!strbuilder_ensure(sb, needed)) {
    va_end(args);
    return 0;
  }

  vsnprintf(sb->data + sb->len, needed + 1, fmt, args);
  sb->len += needed;
  va_end(args);
  return 1;
}

/* Get final string — ownership transferred! (set sb->data = NULL after use) */
static inline char *strbuilder_build(strbuilder_t *sb) {
  if (!sb || !sb->data)
    return strdup("");
  /* Optional: shrink to fit */
  if (sb->len + 1 < sb->capacity) {
    sb->data = (char *)realloc(sb->data, sb->len + 1);
    sb->capacity = sb->len + 1;
  }
  return sb->data;
}

/* Get const view — do NOT free */
static inline const char *strbuilder_view(const strbuilder_t *sb) {
  return sb && sb->data ? sb->data : "";
}

/* Clear but keep allocation */
static inline void strbuilder_clear(strbuilder_t *sb) {
  if (sb) {
    sb->len = 0;
    if (sb->data)
      sb->data[0] = '\0';
  }
}

#endif /* STRBUILDER_H */
