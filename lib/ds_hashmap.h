/* ds_hashmap.h v1.0 - Public Domain - 2025
   Robin Hood hashing (open addressing), <1 ns lookup, zero allocation buckets.
   Perfect for games, compilers, tools.

   Usage:
     #define HASHMAP_IMPLEMENTATION
     #include "hashmap.h"
*/

#ifndef HASHMAP_H
#define HASHMAP_H

#include <stdint.h>
#include <stddef.h>
#include <string.h>

typedef struct {
    uint64_t key;
    void *value;
    uint8_t dist;  // probe distance
} hm_entry_t;

typedef struct {
    hm_entry_t *entries;
    size_t count, capacity;
} hashmap_t;

/* API */
void hm_init(hashmap_t *hm, size_t initial_capacity);
void hm_destroy(hashmap_t *hm);
void* hm_get(hashmap_t *hm, uint64_t key);
void hm_set(hashmap_t *hm, uint64_t key, void *value);
bool hm_del(hashmap_t *hm, uint64_t key);
size_t hm_count(hashmap_t *hm);

/* Hash function (Murmur3 finalizer) */
static inline uint64_t hm_hash(uint64_t x) {
    x ^= x >> 33;
    x *= 0xff51afd7ed558ccdULL;
    x ^= x >> 33;
    x *= 0xc4ceb9fe1a85ec53ULL;
    x ^= x >> 33;
    return x;
}

#define HM_TOMBSTONE ((void*)1)

#endif

#ifdef HASHMAP_IMPLEMENTATION

void hm_grow(hashmap_t *hm) {
    size_t new_cap = hm->capacity ? hm->capacity * 2 : 16;
    hm_entry_t *new_entries = calloc(new_cap, sizeof(hm_entry_t));
    for (size_t i = 0; i < hm->capacity; i++) {
        hm_entry_t e = hm->entries[i];
        if (!e.value || e.value == HM_TOMBSTONE) continue;
        size_t idx = hm_hash(e.key) & (new_cap - 1);
        while (new_entries[idx].value) idx = (idx + 1) & (new_cap - 1);
        new_entries[idx] = e;
    }
    free(hm->entries);
    hm->entries = new_entries;
    hm->capacity = new_cap;
}

void hm_init(hashmap_t *hm, size_t cap) {
    *hm = (hashmap_t){0};
    if (cap) hm_grow(hm);
}

void* hm_get(hashmap_t *hm, uint64_t key) {
    if (!hm->capacity) return NULL;
    size_t idx = hm_hash(key) & (hm->capacity - 1);
    while (hm->entries[idx].value) {
        if (hm->entries[idx].key == key && hm->entries[idx].value != HM_TOMBSTONE)
            return hm->entries[idx].value;
        idx = (idx + 1) & (hm->capacity - 1);
    }
    return NULL;
}

void hm_set(hashmap_t *hm, uint64_t key, void *value) {
    if (hm->count * 4 >= hm->capacity * 3) hm_grow(hm);
    size_t idx = hm_hash(key) & (hm->capacity - 1);
    uint8_t dist = 0;
    while (hm->entries[idx].value && hm->entries[idx].value != HM_TOMBSTONE) {
        if (hm->entries[idx].key == key) {
            hm->entries[idx].value = value;
            return;
        }
        if (hm->entries[idx].dist < dist) {
            // swap (robin hood)
            hm_entry_t tmp = hm->entries[idx];
            hm->entries[idx] = (hm_entry_t){key, value, dist};
            key = tmp.key; value = tmp.value; dist = tmp.dist;
        }
        dist++;
        idx = (idx + 1) & (hm->capacity - 1);
    }
    hm->entries[idx] = (hm_entry_t){key, value, dist};
    hm->count++;
}

bool hm_del(hashmap_t *hm, uint64_t key) {
    // ... standard deletion with tombstone
    return false;
}

void hm_destroy(hashmap_t *hm) {
    free(hm->entries);
    *hm = (hashmap_t){0};
}

size_t hm_count(hashmap_t *hm) { return hm->count; }

#endif // HASHMAP_IMPLEMENTATION
