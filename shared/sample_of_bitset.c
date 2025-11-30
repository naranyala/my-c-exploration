#define BITSET(name, nbits)                                                    \
  uint64_t name##_data[(nbits + 63) / 64] = {0};                               \
  enum { name##_size = nbits }

#define BIT_SET(bs, i) (bs##_data[(i) / 64] |= (1ULL << ((i) % 64)))
#define BIT_CLEAR(bs, i) (bs##_data[(i) / 64] &= ~(1ULL << ((i) % 64)))
#define BIT_TEST(bs, i) ((bs##_data[(i) / 64] >> ((i) % 64)) & 1)
