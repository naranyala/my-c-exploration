#ifndef STR_BUILDER_SIMPLE_H
#define STR_BUILDER_SIMPLE_H

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  char *data;
  size_t len;
  size_t cap;
} str_builder;

#define SB_INIT_CAP 64

static str_builder sb_create(void) { return (str_builder){NULL, 0, 0}; }

static void sb_free(str_builder *sb) {
  free(sb->data);
  sb->data = NULL;
  sb->len = sb->cap = 0;
}

static void sb_append(str_builder *sb, const char *str) {
  if (!str)
    return;

  size_t len = strlen(str);
  if (sb->len + len + 1 > sb->cap) {
    size_t new_cap = sb->cap ? sb->cap * 2 : SB_INIT_CAP;
    while (new_cap < sb->len + len + 1)
      new_cap *= 2;

    sb->data = realloc(sb->data, new_cap);
    sb->cap = new_cap;
  }

  memcpy(sb->data + sb->len, str, len);
  sb->len += len;
  sb->data[sb->len] = '\0';
}

static void sb_append_char(str_builder *sb, char c) {
  if (sb->len + 2 > sb->cap) {
    size_t new_cap = sb->cap ? sb->cap * 2 : SB_INIT_CAP;
    sb->data = realloc(sb->data, new_cap);
    sb->cap = new_cap;
  }

  sb->data[sb->len++] = c;
  sb->data[sb->len] = '\0';
}

static void sb_append_format(str_builder *sb, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);

  // Get required size
  va_list args_copy;
  va_copy(args_copy, args);
  int needed = vsnprintf(NULL, 0, fmt, args_copy);
  va_end(args_copy);

  if (needed > 0) {
    if (sb->len + needed + 1 > sb->cap) {
      size_t new_cap = sb->cap ? sb->cap * 2 : SB_INIT_CAP;
      while (new_cap < sb->len + needed + 1)
        new_cap *= 2;
      sb->data = realloc(sb->data, new_cap);
      sb->cap = new_cap;
    }

    vsnprintf(sb->data + sb->len, needed + 1, fmt, args);
    sb->len += needed;
  }

  va_end(args);
}

static char *sb_string(const str_builder *sb) {
  return sb->data ? sb->data : "";
}

static size_t sb_length(const str_builder *sb) { return sb->len; }

#endif
