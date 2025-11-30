
// fileutil.h
#ifndef FILEUTIL_H
#define FILEUTIL_H

#include <stdio.h>
#include <stdlib.h>

static inline char *read_file(const char *path) {
  FILE *f = fopen(path, "rb");
  if (!f)
    return NULL;
  fseek(f, 0, SEEK_END);
  long len = ftell(f);
  rewind(f);
  char *buf = malloc(len + 1);
  fread(buf, 1, len, f);
  buf[len] = '\0';
  fclose(f);
  return buf;
}

static inline int write_file(const char *path, const char *data) {
  FILE *f = fopen(path, "wb");
  if (!f)
    return -1;
  fputs(data, f);
  fclose(f);
  return 0;
}

#endif
