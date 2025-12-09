// proper_hash.h
#ifndef PROPER_HASH_H
#define PROPER_HASH_H

#include <stddef.h>
#include <stdint.h>

// FNV-1a hash (32-bit) – great for strings and small data
static inline uint32_t proper_hash_fnv1a(const void *data, size_t len) {
  const uint8_t *d = (const uint8_t *)data;
  uint32_t hash = 2166136261u; // FNV offset basis
  for (size_t i = 0; i < len; ++i) {
    hash ^= d[i];
    hash *= 16777619u; // FNV prime
  }
  return hash;
}

// DJB2 hash – classic for null-terminated strings
static inline uint32_t proper_hash_djb2(const char *str) {
  uint32_t hash = 5381;
  int c;
  while ((c = *str++))
    hash = ((hash << 5) + hash) + c; // hash * 33 + c
  return hash;
}

// Simple compile-time string hash macro (DJB2)
#define PROPER_HASH_STR(s)                                                     \
  (__builtin_constant_p(s) ? (__extension__({                                  \
    const char *_s = (s);                                                      \
    uint32_t _h = 5381;                                                        \
    for (size_t _i = 0; _s[_i]; _i++) {                                        \
      _h = ((_h << 5) + _h) + _s[_i];                                          \
    }                                                                          \
    _h;                                                                        \
  }))                                                                          \
                           : proper_hash_djb2(s))

#endif // PROPER_HASH_H
