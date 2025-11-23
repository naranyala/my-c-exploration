/* ergo_vec.h */
#pragma once
#include <stdlib.h>
#include <string.h>

#define vec_t(T) struct { T *data; size_t len, cap; }
#define vec_new(T) ((vec_t(T)){0})
#define vec_free(v) do { free((v).data); (v).data = NULL; (v).len = (v).cap = 0; } while(0)

#define vec_reserve(v, need) do { \
    if ((v).cap < (need)) { \
        size_t newcap = (v).cap ? (v).cap * 2 : 8; \
        if (newcap < (need)) newcap = (need); \
        (v).data = realloc((v).data, newcap * sizeof(*(v).data)); \
        (v).cap = newcap; \
    } \
} while(0)

#define vec_push(v, val) do { \
    vec_reserve(v, (v).len + 1); \
    (v).data[(v).len++] = (val); \
} while(0)

#define vec_pop(v) ((v).data[--(v).len])
#define vec_last(v) ((v).data[(v).len-1])

