
// safearray.h
#ifndef SAFEARRAY_H
#define SAFEARRAY_H

#include <stdio.h>

#define SAFE_GET(arr, idx, size)                                               \
  (((idx) >= 0 && (idx) < (size))                                              \
       ? (arr)[(idx)]                                                          \
       : (fprintf(stderr, "Index out of bounds\n"), 0))

#endif
