
// cleanup.h
#ifndef CLEANUP_H
#define CLEANUP_H

#include <stdlib.h>

#define CLEANUP(ptr)                                                           \
  do {                                                                         \
    if (ptr) {                                                                 \
      free(ptr);                                                               \
      ptr = NULL;                                                              \
    }                                                                          \
  } while (0)

#endif
