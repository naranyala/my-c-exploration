
// math_utils.h
#ifndef MATHUTIL_H
#define MATHUTIL_H

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define CLAMP(x, low, high)                                                    \
  (((x) < (low)) ? (low) : ((x) > (high)) ? (high) : (x))

#endif
