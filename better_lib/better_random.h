#include <stdlib.h>

/* better_random.h - No dependencies, cryptographically okay for games/simulations */
static uint64_t brng_state[2] = { 0x1e9a7f7e, 0x9d2e3f4a };

static inline uint64_t brng_next(void) {
    uint64_t s1 = brng_state[0];
    uint64_t s0 = brng_state[1];
    brng_state[0] = s0;
    s1 ^= s1 << 23;
    brng_state[1] = s1 ^ s0 ^ (s1 >> 17) ^ (s0 >> 26);
    return brng_state[1] + s0;
}

#define brng_int() ((int)(brng_next() >> 33))
#define brng_float() (brng_next() * (1.0 / 18446744073709551616.0))
