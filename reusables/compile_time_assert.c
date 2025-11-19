#ifndef static_assert
#define static_assert _Static_assert
#endif

// Usage
static_assert(sizeof(long) == 8, "Need 64-bit long");
