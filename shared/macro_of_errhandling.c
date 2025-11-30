#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DIE(...)                                                               \
  do {                                                                         \
    fprintf(stderr, "Error: " __VA_ARGS__);                                    \
    fprintf(stderr, "\n");                                                     \
    exit(EXIT_FAILURE);                                                        \
  } while (0)

#define EDIE(...)                                                              \
  do {                                                                         \
    fprintf(stderr, "Error: " __VA_ARGS__);                                    \
    fprintf(stderr, ": %s\n", strerror(errno));                                \
    exit(EXIT_FAILURE);                                                        \
  } while (0)

#define CHECK(cond, ...)                                                       \
  do {                                                                         \
    if (!(cond))                                                               \
      DIE(__VA_ARGS__);                                                        \
  } while (0)
