/* mylib.h */
#ifndef MYLIB_H
#define MYLIB_H

#include <stddef.h>

static inline int max(int a, int b) { return a > b ? a : b; }

typedef struct {
  ...
} MyType;

MyType *mytype_new(void);
void mytype_free(MyType *t);

#endif /* MYLIB_H */
