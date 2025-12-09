// proper_rand.h
#ifndef PROPER_RAND_H
#define PROPER_RAND_H

#include <stdint.h>
#include <time.h>

// Xorshift32 – fast, decent quality, 32-bit state
typedef struct {
  uint32_t state;
} proper_rand_t;

static inline void proper_rand_init(proper_rand_t *r, uint32_t seed) {
  if (seed == 0)
    seed = (uint32_t)time(NULL);
  r->state = seed;
}

static inline void proper_rand_seed_time(proper_rand_t *r) {
  proper_rand_init(r, (uint32_t)time(NULL));
}

static inline uint32_t proper_rand_next(proper_rand_t *r) {
  uint32_t x = r->state;
  x ^= x << 13;
  x ^= x >> 17;
  x ^= x << 5;
  r->state = x;
  return x;
}

// Returns [0, 1)
static inline double proper_rand_unit(proper_rand_t *r) {
  return proper_rand_next(r) / ((double)UINT32_MAX + 1.0);
}

// Returns [min, max] (inclusive)
static inline int proper_rand_range(proper_rand_t *r, int min, int max) {
  if (min > max) {
    int t = min;
    min = max;
    max = t;
  }
  uint32_t range = (uint32_t)(max - min + 1);
  return min + (proper_rand_next(r) % range);
}

#endif // PROPER_RAND_H
