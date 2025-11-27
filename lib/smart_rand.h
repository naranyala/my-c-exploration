#ifndef SMART_RAND_H
#define SMART_RAND_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

/* ---------------------------------------------------------
   State
   --------------------------------------------------------- */
typedef struct {
    uint64_t s[2];
} sr128_state;

/* Simple splitmix64 seed */
static uint64_t sr_splitmix64(uint64_t* x) {
    uint64_t z = (*x += 0x9E3779B97F4A7C15ULL);
    z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ULL;
    z = (z ^ (z >> 27)) * 0x94D049BB133111EBULL;
    return z ^ (z >> 31);
}

/* ---------------------------------------------------------
   Initialization
   --------------------------------------------------------- */
static sr128_state sr_init(uint64_t seed) {
    sr128_state st;
    st.s[0] = sr_splitmix64(&seed);
    st.s[1] = sr_splitmix64(&seed);
    return st;
}

/* ---------------------------------------------------------
   xoroshiro128+  (fast & high-quality)
   --------------------------------------------------------- */
static inline uint64_t sr_rotl(uint64_t x, int k) {
    return (x << k) | (x >> (64 - k));
}

static uint64_t sr_xoroshiro128plus(sr128_state* st) {
    uint64_t s0 = st->s[0];
    uint64_t s1 = st->s[1];
    uint64_t result = s0 + s1;

    s1 ^= s0;
    st->s[0] = sr_rotl(s0, 55) ^ s1 ^ (s1 << 14);
    st->s[1] = sr_rotl(s1, 36);

    return result;
}

/* ---------------------------------------------------------
   Integer & float helpers
   --------------------------------------------------------- */
static uint64_t sr_rand_u64(sr128_state* st) {
    return sr_xoroshiro128plus(st);
}

static uint32_t sr_rand_u32(sr128_state* st) {
    return (uint32_t)sr_xoroshiro128plus(st);
}

static float sr_rand_float(sr128_state* st) {
    return (sr_rand_u32(st) >> 8) * (1.0f / 16777216.0f);
}

static float sr_rand_rangef(sr128_state* st, float min, float max) {
    return min + sr_rand_float(st) * (max - min);
}

static int sr_rand_rangei(sr128_state* st, int min, int max) {
    if (min > max) {
        // Swap if min > max
        int temp = min;
        min = max;
        max = temp;
    }
    uint32_t range = max - min + 1;
    uint32_t r = sr_rand_u32(st);
    return min + (int)(r % range);
}

/* ---------------------------------------------------------
   Choice helper (pick one from array)
   --------------------------------------------------------- */
static const void* sr_choice(sr128_state* st,
                             const void* array,
                             int count, int elem_size) {
    if (count <= 0) return NULL;
    int idx = sr_rand_rangei(st, 0, count - 1);
    return (const uint8_t*)array + (idx * elem_size);
}

#ifdef __cplusplus
}
#endif

#endif /* SMART_RAND_H */
