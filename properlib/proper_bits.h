// proper_bits.h
#ifndef PROPER_BITS_H
#define PROPER_BITS_H

#include <stdint.h>

// Count leading zeros (32-bit)
static inline int proper_clz32(uint32_t x) {
#ifdef __GNUC__
  return x ? __builtin_clz(x) : 32;
#else
  if (x == 0)
    return 32;
  int n = 0;
  if (x <= 0x0000FFFF) {
    n += 16;
    x <<= 16;
  }
  if (x <= 0x00FFFFFF) {
    n += 8;
    x <<= 8;
  }
  if (x <= 0x0FFFFFFF) {
    n += 4;
    x <<= 4;
  }
  if (x <= 0x3FFFFFFF) {
    n += 2;
    x <<= 2;
  }
  if (x <= 0x7FFFFFFF) {
    n += 1;
  }
  return n;
#endif
}

// Is power of two?
static inline int proper_is_power_of_two_u32(uint32_t x) {
  return x && !(x & (x - 1));
}

// Round up to next power of two (32-bit)
static inline uint32_t proper_round_up_pow2_u32(uint32_t v) {
  if (v == 0)
    return 1;
  v--;
  v |= v >> 1;
  v |= v >> 2;
  v |= v >> 4;
  v |= v >> 8;
  v |= v >> 16;
  return v + 1;
}

// Toggle, set, clear, test bit
#define PROPER_BIT_SET(x, b) ((x) |= (1U << (b)))
#define PROPER_BIT_CLEAR(x, b) ((x) &= ~(1U << (b)))
#define PROPER_BIT_TOGGLE(x, b) ((x) ^= (1U << (b)))
#define PROPER_BIT_TEST(x, b) (((x) >> (b)) & 1U)

#endif // PROPER_BITS_H
