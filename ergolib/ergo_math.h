#ifndef ERGO_MATH_H
#define ERGO_MATH_H

#include <math.h>

// Clamp value between min and max
static inline double ergo_clamp(double x, double min, double max) {
    return (x < min) ? min : (x > max) ? max : x;
}

// Linear interpolation
static inline double ergo_lerp(double a, double b, double t) {
    return a + (b - a) * t;
}

// Map value from one range to another
static inline double ergo_map(double x, double in_min, double in_max, double out_min, double out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

#endif // ERGO_MATH_H

