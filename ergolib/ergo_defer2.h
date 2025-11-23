/* ergo_defer.h */
#pragma once
#include <stdio.h>

typedef struct { void (*fn)(void *); void *arg; } ergo_defer_t;

#define ergo_concat_impl(a, b) a##b
#define ergo_concat(a, b) ergo_concat_impl(a, b)

#define defer_block(block) \
    for (int ergo_concat(_i_, __LINE__) = 0; ergo_concat(_i_, __LINE__) < 1; ergo_concat(_i_, __LINE__)++)

#define defer __attribute__((cleanup(ergo_defer_run))) ergo_defer_t ergo_concat(_defer_, __LINE__); \
              defer_block({ ergo_concat(_defer_, __LINE__).fn = (void(*)(void*))0; })

#define ergo_defer_run(p) do { \
    if ((p)->fn) ((void(*)(void*))(p)->fn)((p)->arg); \
} while(0)

#define DEFER(func) defer = (ergo_defer_t){ .fn = (void(*)(void*))func, .arg = NULL }
#define DEFER_ARG(func, arg) defer = (ergo_defer_t){ .fn = (void(*)(void*))func, .arg = (void*)arg }

