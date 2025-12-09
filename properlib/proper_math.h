// proper_math.h
#ifndef PROPER_MATH_H
#define PROPER_MATH_H

#include <math.h>

// Float utilities
static inline float proper_clamp_f(float x, float min, float max) {
  return x < min ? min : (x > max ? max : x);
}

static inline float proper_lerp_f(float a, float b, float t) {
  return a + t * (b - a);
}

static inline float proper_deg_to_rad(float deg) {
  return deg * (3.14159265358979323846f / 180.0f);
}
static inline float proper_rad_to_deg(float rad) {
  return rad * (180.0f / 3.14159265358979323846f);
}

// 2D Vector
typedef struct {
  float x, y;
} proper_vec2;
static inline proper_vec2 proper_vec2_add(proper_vec2 a, proper_vec2 b) {
  return (proper_vec2){a.x + b.x, a.y + b.y};
}
static inline proper_vec2 proper_vec2_sub(proper_vec2 a, proper_vec2 b) {
  return (proper_vec2){a.x - b.x, a.y - b.y};
}
static inline float proper_vec2_dot(proper_vec2 a, proper_vec2 b) {
  return a.x * b.x + a.y * b.y;
}
static inline float proper_vec2_len2(proper_vec2 v) {
  return proper_vec2_dot(v, v);
}
static inline float proper_vec2_len(proper_vec2 v) {
  return sqrtf(proper_vec2_len2(v));
}

// Integer utilities
static inline int proper_min_i(int a, int b) { return a < b ? a : b; }
static inline int proper_max_i(int a, int b) { return a > b ? a : b; }

// Round float to nearest int
static inline int proper_round_i(float x) {
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
  return (int)roundf(x);
#else
  return (int)(x >= 0.0f ? x + 0.5f : x - 0.5f);
#endif
}

#endif // PROPER_MATH_H
