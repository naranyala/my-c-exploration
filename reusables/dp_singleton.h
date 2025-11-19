
// singleton.h
#ifndef SINGLETON_H
#define SINGLETON_H

#include <stdlib.h>

typedef struct {
  int value;
} Singleton;

static inline Singleton *get_instance() {
  static Singleton *inst = NULL;
  if (!inst) {
    inst = malloc(sizeof(Singleton));
    inst->value = 0;
  }
  return inst;
}

#endif
