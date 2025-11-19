
// error.h
#ifndef ERROR_H
#define ERROR_H

#include <stdio.h>
#include <stdlib.h>

#define CHECK(cond, msg)                                                       \
  do {                                                                         \
    if (!(cond)) {                                                             \
      fprintf(stderr, "Error: %s\n", msg);                                     \
      exit(EXIT_FAILURE);                                                      \
    }                                                                          \
  } while (0)

#endif
