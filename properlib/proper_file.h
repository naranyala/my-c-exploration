// proper_file.h
#ifndef PROPER_FILE_H
#define PROPER_FILE_H

#include <stdio.h>
#include <stdlib.h>

// Read entire file into a null-terminated string (caller must free)
static inline char *proper_file_read(const char *path, size_t *out_len) {
  if (!path)
    return NULL;
  FILE *f = fopen(path, "rb");
  if (!f)
    return NULL;
  fseek(f, 0, SEEK_END);
  long size = ftell(f);
  if (size < 0) {
    fclose(f);
    return NULL;
  }
  rewind(f);
  char *buf = (char *)malloc((size_t)size + 1);
  if (!buf) {
    fclose(f);
    return NULL;
  }
  if (fread(buf, 1, (size_t)size, f) != (size_t)size) {
    free(buf);
    fclose(f);
    return NULL;
  }
  buf[size] = '\0';
  fclose(f);
  if (out_len)
    *out_len = (size_t)size;
  return buf;
}

#endif // PROPER_FILE_H
