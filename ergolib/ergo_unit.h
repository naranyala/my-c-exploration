
#ifndef ERGO_UNIT_H
#define ERGO_UNIT_H

static inline long ergo_ms_to_us(long ms) { return ms * 1000; }
static inline long ergo_sec_to_ms(long s) { return s * 1000; }
static inline long ergo_min_to_ms(long m) { return m * 60000; }

static inline long ergo_kb(long b) { return b * 1024; }
static inline long ergo_mb(long b) { return b * 1024 * 1024; }

#endif
