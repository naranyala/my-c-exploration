/* ergo_hashmap.h */
#pragma once
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    uint32_t *keys;     /* hashed keys */
    char     *strs;     /* concatenated keys */
    void    **vals;
    size_t    cap, len;
} hmap_t;

#define hmap_new() ((hmap_t){0})
#define hmap_free(m) do { free((m).keys); free((m).strs); free((m).vals); } while(0)

static inline uint32_t hmap_hash(const char *s) {
    uint32_t h = 5381;
    int c;
    while ((c = *s++)) h = ((h << 5) + h) + c;
    return h ? h : 1;
}

static inline void hmap_grow(hmap_t *m) {
    size_t oldcap = m->cap;
    m->cap = m->cap ? m->cap*2 : 16;
    m->keys = realloc(m->keys, m->cap * sizeof(uint32_t));
    m->vals = realloc(m->vals, m->cap * sizeof(void*));
    if (oldcap) {
        char *oldstrs = m->strs;
        m->strs = malloc(m->cap * 32);  /* rough avg key size */
        size_t off = 0;
        for (size_t i = 0; i < oldcap; i++) {
            if (m->keys[i]) {
                size_t klen = strlen(oldstrs + (i*32));
                strcpy(m->strs + off, oldstrs + (i*32));
                m->keys[i] = hmap_hash(m->strs + off);  /* rehash */
                off += klen + 1;
            }
        }
        free(oldstrs);
    } else {
        m->strs = malloc(m->cap * 32);
    }
}

static inline void hmap_put(hmap_t *m, const char *key, void *val) {
    if (m->len+1 > m->cap*0.7) hmap_grow(m);
    uint32_t h = hmap_hash(key);
    size_t i = h & (m->cap-1);
    while (m->keys[i] && m->keys[i] != h) i = (i+1) & (m->cap-1);
    if (!m->keys[i]) {
        strcpy(m->strs + (i*32), key);
        m->keys[i] = h;
        m->len++;
    }
    m->vals[i] = val;
}

static inline void* hmap_get(hmap_t *m, const char *key) {
    if (!m->cap) return NULL;
    uint32_t h = hmap_hash(key);
    size_t i = h & (m->cap-1);
    while (m->keys[i] && m->keys[i] != h) i = (i+1) & (m->cap-1);
    return m->keys[i] ? m->vals[i] : NULL;
}

