
// safefile.h
#ifndef SAFEFILE_H
#define SAFEFILE_H

#include <stdio.h>

typedef struct {
  FILE *fp;
} SafeFile;

static inline SafeFile sf_open(const char *path, const char *mode) {
  SafeFile sf = {fopen(path, mode)};
  return sf;
}

static inline void sf_close(SafeFile *sf) {
  if (sf->fp)
    fclose(sf->fp);
  sf->fp = NULL;
}

#endif
