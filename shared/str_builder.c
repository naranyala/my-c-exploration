#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  char *buffer;
  size_t length;
  size_t capacity;
} StringBuilder;

StringBuilder *sb_create() {
  StringBuilder *sb = malloc(sizeof(StringBuilder));
  sb->capacity = 16;
  sb->length = 0;
  sb->buffer = malloc(sb->capacity);
  sb->buffer[0] = '\0';
  return sb;
}

void sb_resize_if_needed(StringBuilder *sb, size_t needed_length) {
  if (needed_length >= sb->capacity) {
    sb->capacity = (needed_length + 1) * 2;
    sb->buffer = realloc(sb->buffer, sb->capacity);
  }
}

void sb_append(StringBuilder *sb, const char *str) {
  size_t str_len = strlen(str);
  size_t needed_length = sb->length + str_len;

  sb_resize_if_needed(sb, needed_length);

  strcat(sb->buffer, str);
  sb->length = needed_length;
}

char *sb_to_string(StringBuilder *sb) {
  return strdup(sb->buffer); // Returns a copy
}

void sb_free(StringBuilder *sb) {
  free(sb->buffer);
  free(sb);
}

int main() {
  StringBuilder *sb = sb_create();
  sb_append(sb, "Hello ");
  sb_append(sb, "World ");
  sb_append(sb, "!");

  char *result = sb_to_string(sb);
  printf("%s\n", result);

  free(result);
  sb_free(sb);
  return 0;
}
