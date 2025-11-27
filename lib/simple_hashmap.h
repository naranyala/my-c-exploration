#ifndef SIMPLE_HASHMAP_H
#define SIMPLE_HASHMAP_H

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct hashmap_s hashmap_t;
typedef uint64_t hashmap_hash_t;

// Create new hashmap with initial capacity.
// Returns NULL on failure.
hashmap_t* hashmap_new(size_t capacity);

// Insert key-value pair. Returns 0 on success.
int hashmap_insert(hashmap_t* map, const char* key, void* value);

// Get value by key. Returns NULL if not found.
void* hashmap_get(hashmap_t* map, const char* key);

// Remove key. Returns 1 if found and removed, 0 otherwise.
int hashmap_remove(hashmap_t* map, const char* key);

// Free hashmap and all entries.
void hashmap_free(hashmap_t* map);

#ifdef __cplusplus
}
#endif

#endif // SIMPLE_HASHMAP_H

#ifdef SIMPLE_HASHMAP_IMPLEMENTATION

#define HASHMAP_DEFAULT_CAPACITY 16
#define HASHMAP_MAX_LOAD 0.75f

typedef struct hashmap_entry_s {
    char* key;
    void* value;
    struct hashmap_entry_s* next;
} hashmap_entry_t;

struct hashmap_s {
    hashmap_entry_t** buckets;
    size_t capacity;
    size_t size;
};

static hashmap_hash_t hash_string(const char* str) {
    hashmap_hash_t hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

hashmap_t* hashmap_new(size_t capacity) {
    if (capacity < HASHMAP_DEFAULT_CAPACITY) capacity = HASHMAP_DEFAULT_CAPACITY;
    
    hashmap_t* map = (hashmap_t*)calloc(1, sizeof(hashmap_t));
    if (!map) return NULL;
    
    map->capacity = capacity;
    map->buckets = (hashmap_entry_t**)calloc(capacity, sizeof(hashmap_entry_t*));
    if (!map->buckets) {
        free(map);
        return NULL;
    }
    return map;
}

static int resize(hashmap_t* map) {
    size_t new_cap = map->capacity * 2;
    hashmap_entry_t** new_buckets = (hashmap_entry_t**)calloc(new_cap, sizeof(hashmap_entry_t*));
    if (!new_buckets) return -1;
    
    for (size_t i = 0; i < map->capacity; i++) {
        hashmap_entry_t* entry = map->buckets[i];
        while (entry) {
            hashmap_entry_t* next = entry->next;
            size_t idx = hash_string(entry->key) % new_cap;
            entry->next = new_buckets[idx];
            new_buckets[idx] = entry;
            entry = next;
        }
    }
    
    free(map->buckets);
    map->buckets = new_buckets;
    map->capacity = new_cap;
    return 0;
}

int hashmap_insert(hashmap_t* map, const char* key, void* value) {
    if (!map || !key) return -1;
    
    if ((float)map->size / map->capacity > HASHMAP_MAX_LOAD) {
        if (resize(map) != 0) return -1;
    }
    
    size_t idx = hash_string(key) % map->capacity;
    hashmap_entry_t* entry;
    
    // Check if key exists
    for (entry = map->buckets[idx]; entry; entry = entry->next) {
        if (strcmp(entry->key, key) == 0) {
            entry->value = value;
            return 0;
        }
    }
    
    // New entry
    entry = (hashmap_entry_t*)malloc(sizeof(hashmap_entry_t));
    if (!entry) return -1;
    
    entry->key = strdup(key);
    entry->value = value;
    entry->next = map->buckets[idx];
    map->buckets[idx] = entry;
    map->size++;
    return 0;
}

void* hashmap_get(hashmap_t* map, const char* key) {
    if (!map || !key) return NULL;
    
    size_t idx = hash_string(key) % map->capacity;
    for (hashmap_entry_t* entry = map->buckets[idx]; entry; entry = entry->next) {
        if (strcmp(entry->key, key) == 0) {
            return entry->value;
        }
    }
    return NULL;
}

int hashmap_remove(hashmap_t* map, const char* key) {
    if (!map || !key) return 0;
    
    size_t idx = hash_string(key) % map->capacity;
    hashmap_entry_t** cur = &map->buckets[idx];
    
    while (*cur) {
        if (strcmp((*cur)->key, key) == 0) {
            hashmap_entry_t* to_free = *cur;
            *cur = (*cur)->next;
            free(to_free->key);
            free(to_free);
            map->size--;
            return 1;
        }
        cur = &(*cur)->next;
    }
    return 0;
}

void hashmap_free(hashmap_t* map) {
    if (!map) return;
    
    for (size_t i = 0; i < map->capacity; i++) {
        hashmap_entry_t* entry = map->buckets[i];
        while (entry) {
            hashmap_entry_t* next = entry->next;
            free(entry->key);
            free(entry);
            entry = next;
        }
    }
    free(map->buckets);
    free(map);
}

#endif // SIMPLE_HASHMAP_IMPLEMENTATION

