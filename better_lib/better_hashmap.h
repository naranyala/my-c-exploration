/* better_hashmap.h v1.1 - String → void* map, grow-on-demand */
#ifndef BETTER_HASHMAP_H
#define BETTER_HASHMAP_H
#include <stdint.h>
#include <string.h>

typedef struct { char* key; void* value; uint32_t hash; } bhmentry;

typedef struct {
    bhmentry* entries;
    size_t count, capacity;
} bhmap;

static uint32_t bhm_hash(const char* s) {
    uint32_t h = 5381;
    int c; while ((c = *s++)) h = ((h << 5) + h) + c;
    return h;
}

static inline void* bhm_get(bhmap* m, const char* key) {
    if (!m->entries) return NULL;
    uint32_t h = bhm_hash(key);
    size_t i = h & (m->capacity - 1);
    while (m->entries[i].hash && strcmp(m->entries[i].key, key) != 0)
        i = (i + 1) & (m->capacity - 1);
    return m->entries[i].hash ? m->entries[i].value : NULL;
}

/* Insert + full implementation available on request — this is the hot path */
#endif
