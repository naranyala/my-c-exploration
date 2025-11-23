/* better_vec.h v1.2 - Like C++ std::vector, but for C */
#ifndef BETTER_VEC_H
#define BETTER_VEC_H
#include <stdlib.h>
#include <string.h>

#define Vec(T)          T*
#define vec_new(T)      ((Vec(T))malloc(sizeof(T) * 8))
#define vec_push(v, val) do { \
    if (!(v) || vec_count(v) + 1 > vec_cap(v)) \
        (v) = vec_grow(v, sizeof(*(v))); \
    (v)[vec_count(v)++] = (val); \
} while(0)

static inline void* vec_grow(void* v, size_t esize) {
    size_t* meta = (size_t*)v - 2;
    size_t cap = v ? meta[0] : 0;
    size_t newcap = cap ? cap * 2 : 8;
    size_t* newmeta = realloc(v ? meta : NULL, sizeof(size_t)*2 + newcap*esize);
    newmeta[0] = newcap; newmeta[1] = v ? meta[1] : 0;
    return newmeta + 2;
}
#define vec_count(v) ((v) ? ((size_t*)(v))[-1] : 0)
#define vec_cap(v)   ((v) ? ((size_t*)(v))[-2] : 0)
#define vec_free(v)  do { if(v) free(((size_t*)(v))-2); } while(0)

#endif
