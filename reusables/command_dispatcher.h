
// dispatcher.h
#ifndef DISPATCHER_H
#define DISPATCHER_H

#include <string.h>

typedef void (*CommandFunc)(void);

typedef struct {
  const char *name;
  CommandFunc func;
} Command;

static inline void dispatch(Command *cmds, size_t n, const char *input) {
  for (size_t i = 0; i < n; i++) {
    if (strcmp(cmds[i].name, input) == 0) {
      cmds[i].func();
      return;
    }
  }
}

#endif
