#ifndef ERGO_SLICE_H
#define ERGO_SLICE_H

#include <stddef.h>

typedef struct {
    void *data;
    size_t len;
    size_t stride;
} ergo_slice;

#define ergo_slice_of(arr) \
    (ergo_slice){ (void*)(arr), sizeof(arr)/sizeof(arr[0]), sizeof((arr)[0]) }

static inline void *ergo_slice_at(ergo_slice s, size_t i) {
    return (char*)s.data + (i * s.stride);
}

#endif

