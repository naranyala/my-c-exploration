#ifndef ERGO_FLAGS_H
#define ERGO_FLAGS_H

#include <stdint.h>

#define ergo_flag32 uint32_t

#define ergo_flag_set(v, f)   ((v) |= (f))
#define ergo_flag_clear(v,f)  ((v) &= ~(f))
#define ergo_flag_has(v,f)    (((v) & (f)) != 0)

#endif

