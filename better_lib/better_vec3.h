
/* better_vec.h — tiny vector */

#ifndef BETTER_VEC_H
#define BETTER_VEC_H

#include <stdlib.h>
#include <string.h>

typedef struct {
    size_t len;
    size_t cap;
    void *data;
} bvec;

static inline void bvec_init(bvec *v) {
    v->len = v->cap = 0;
    v->data = NULL;
}

static inline void bvec_free(bvec *v) {
    free(v->data);
    v->data = NULL;
    v->len = v->cap = 0;
}

static inline int bvec_reserve(bvec *v, size_t newcap, size_t elem_size) {
    if (newcap <= v->cap) return 1;
    void *p = realloc(v->data, newcap * elem_size);
    if (!p) return 0;
    v->data = p;
    v->cap = newcap;
    return 1;
}

static inline int bvec_push(bvec *v, const void *elem, size_t elem_size) {
    if (v->len + 1 > v->cap) {
        size_t newcap = v->cap ? v->cap * 2 : 4;
        if (!bvec_reserve(v, newcap, elem_size)) return 0;
    }
    memcpy((char*)v->data + v->len * elem_size, elem, elem_size);
    v->len++;
    return 1;
}

#endif /* BETTER_VEC_H */
