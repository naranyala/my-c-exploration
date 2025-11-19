
// cli.h
#ifndef CLI_H
#define CLI_H

#include <string.h>

static inline const char *get_arg(int argc, char **argv, const char *flag) {
  for (int i = 1; i < argc - 1; i++) {
    if (strcmp(argv[i], flag) == 0) {
      return argv[i + 1];
    }
  }
  return NULL;
}

#endif
