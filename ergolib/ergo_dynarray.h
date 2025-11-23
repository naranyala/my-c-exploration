#ifndef ERGO_DYNARRAY_H
#define ERGO_DYNARRAY_H

#include <stdlib.h>
#include <string.h>

typedef struct {
    void *data;
    size_t elem_size;
    size_t len;
    size_t cap;
} ergo_dynarray;

static inline void ergo_dynarray_init(ergo_dynarray *a, size_t elem_size) {
    a->data = NULL; a->elem_size = elem_size; a->len = 0; a->cap = 0;
}

static inline void ergo_dynarray_push(ergo_dynarray *a, void *elem) {
    if (a->len == a->cap) {
        a->cap = a->cap ? a->cap * 2 : 4;
        a->data = realloc(a->data, a->cap * a->elem_size);
    }
    memcpy((char*)a->data + a->len * a->elem_size, elem, a->elem_size);
    a->len++;
}

static inline void *ergo_dynarray_get(ergo_dynarray *a, size_t i) {
    return (char*)a->data + i * a->elem_size;
}

static inline void ergo_dynarray_free(ergo_dynarray *a) {
    free(a->data);
}

#endif // ERGO_DYNARRAY_H

