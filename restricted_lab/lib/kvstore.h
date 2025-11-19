#ifndef KVSTORE_H
#define KVSTORE_H

#include "raylib.h" // For Vector2
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- Types ---

typedef enum {
  KV_NONE,
  KV_INT,
  KV_FLOAT,
  KV_BOOL,
  KV_STRING,
  KV_VEC2,
  KV_PTR
} KVType;

typedef struct {
  KVType type;
  union {
    int i;
    float f;
    bool b;
    char *s;
    Vector2 v;
    void *p;
  } as;
} KVValue;

typedef struct {
  char *key;
  KVValue value;
  bool occupied;
  bool tombstone;
} KVEntry;

typedef struct KVStore {
  KVEntry *entries;
  int capacity;
  int count;
} KVStore;

// --- Internal Helpers ---

// FNV-1a Hash
static inline uint32_t kv_hash(const char *key) {
  uint32_t hash = 2166136261u;
  while (*key) {
    hash ^= (uint8_t)(*key++);
    hash *= 16777619u;
  }
  return hash;
}

static inline void kv_free_value(KVValue *val) {
  if (val->type == KV_STRING && val->as.s != NULL) {
    free(val->as.s);
    val->as.s = NULL;
  }
}

static inline KVStore *KV_Create(void) {
  KVStore *store = (KVStore *)malloc(sizeof(KVStore));
  if (!store)
    return NULL;
  store->capacity = 16; // Initial capacity
  store->count = 0;
  store->entries = (KVEntry *)calloc(store->capacity, sizeof(KVEntry));
  return store;
}

static inline void KV_Destroy(KVStore *store) {
  if (!store)
    return;
  for (int i = 0; i < store->capacity; i++) {
    if (store->entries[i].occupied) {
      free(store->entries[i].key);
      kv_free_value(&store->entries[i].value);
    }
  }
  free(store->entries);
  free(store);
}

// Forward declaration
static inline void KV_Set(KVStore *store, const char *key, KVValue value);

static inline void kv_resize(KVStore *store, int new_capacity) {
  KVEntry *old_entries = store->entries;
  int old_capacity = store->capacity;

  store->entries = (KVEntry *)calloc(new_capacity, sizeof(KVEntry));
  store->capacity = new_capacity;
  store->count = 0; // Re-counting as we re-insert

  for (int i = 0; i < old_capacity; i++) {
    if (old_entries[i].occupied) {
      KV_Set(store, old_entries[i].key, old_entries[i].value);
      // Free the key from the old entry as KV_Set makes a copy
      // But wait, we can optimize by moving ownership?
      // For simplicity and safety in this single-header, let's just let KV_Set
      // copy and then we free the old key/value.
      // Actually, KV_Set takes value by value, so string pointer is copied.
      // We should NOT free the value string here if we passed it to KV_Set.
      // But KV_Set (wrapper) might duplicate string?
      // Let's look at KV_Set implementation below.
      // To avoid double free or complex ownership, let's just deep copy
      // everything during resize is safer but slower. OPTIMIZATION: Move
      // ownership. Since this is internal resize, we know we can just move the
      // pointers. But we need to re-hash.
    }
  }

  // However, implementing "Move" logic inside KV_Set is complex.
  // Let's stick to the standard re-insert.
  // BUT, we must be careful about strings.
  // If KV_Set duplicates the string, we must free the old one.
  // If KV_Set takes ownership, we shouldn't.
  // Let's define KV_Set to take ownership of the value if it's a string?
  // No, standard Set usually copies.
  // Let's implement a raw internal insert that doesn't copy keys/values, just
  // moves.
}

static inline void kv_insert_raw(KVStore *store, char *key, KVValue value) {
  uint32_t hash = kv_hash(key);
  int idx = hash % store->capacity;

  while (true) {
    if (!store->entries[idx].occupied) {
      store->entries[idx].key = key; // Take ownership
      store->entries[idx].value = value;
      store->entries[idx].occupied = true;
      store->entries[idx].tombstone = false;
      store->count++;
      return;
    }
    // If same key, overwrite (shouldn't happen in resize re-insert of unique
    // keys)
    if (strcmp(store->entries[idx].key, key) == 0) {
      kv_free_value(&store->entries[idx].value);
      free(store->entries[idx].key); // Free old key
      store->entries[idx].key = key;
      store->entries[idx].value = value;
      return;
    }
    idx = (idx + 1) % store->capacity;
  }
}

static inline void kv_resize_optimized(KVStore *store, int new_capacity) {
  KVEntry *old_entries = store->entries;
  int old_capacity = store->capacity;

  store->entries = (KVEntry *)calloc(new_capacity, sizeof(KVEntry));
  store->capacity = new_capacity;
  store->count = 0;

  for (int i = 0; i < old_capacity; i++) {
    if (old_entries[i].occupied) {
      // Move ownership of key and value (string ptr) to new store
      kv_insert_raw(store, old_entries[i].key, old_entries[i].value);
    }
  }
  free(old_entries);
}

// --- Public Operations ---

static inline void KV_Set(KVStore *store, const char *key, KVValue value) {
  if ((float)(store->count + 1) / store->capacity > 0.75f) {
    kv_resize_optimized(store, store->capacity * 2);
  }

  uint32_t hash = kv_hash(key);
  int idx = hash % store->capacity;
  int tombstone_idx = -1;

  while (true) {
    if (!store->entries[idx].occupied) {
      if (store->entries[idx].tombstone) {
        if (tombstone_idx == -1)
          tombstone_idx = idx;
      } else {
        // Found empty slot
        int target = (tombstone_idx != -1) ? tombstone_idx : idx;
        store->entries[target].key = strdup(key);

        // Deep copy string value if needed
        if (value.type == KV_STRING && value.as.s != NULL) {
          value.as.s = strdup(value.as.s);
        }

        store->entries[target].value = value;
        store->entries[target].occupied = true;
        store->entries[target].tombstone = false;
        store->count++;
        return;
      }
    } else if (strcmp(store->entries[idx].key, key) == 0) {
      // Overwrite
      kv_free_value(&store->entries[idx].value);

      // Deep copy string value if needed
      if (value.type == KV_STRING && value.as.s != NULL) {
        value.as.s = strdup(value.as.s);
      }

      store->entries[idx].value = value;
      return;
    }

    idx = (idx + 1) % store->capacity;
    // In a full loop (shouldn't happen due to resize), we'd break.
    // But resize guarantees space.
  }
}

static inline KVValue KV_Get(KVStore *store, const char *key) {
  if (!store)
    return (KVValue){.type = KV_NONE};

  uint32_t hash = kv_hash(key);
  int idx = hash % store->capacity;
  int start_idx = idx;

  while (true) {
    if (!store->entries[idx].occupied && !store->entries[idx].tombstone) {
      return (KVValue){.type = KV_NONE};
    }
    if (store->entries[idx].occupied &&
        strcmp(store->entries[idx].key, key) == 0) {
      return store->entries[idx].value;
    }
    idx = (idx + 1) % store->capacity;
    if (idx == start_idx)
      return (KVValue){.type = KV_NONE};
  }
}

static inline bool KV_Has(KVStore *store, const char *key) {
  return KV_Get(store, key).type != KV_NONE;
}

static inline bool KV_Delete(KVStore *store, const char *key) {
  if (!store)
    return false;

  uint32_t hash = kv_hash(key);
  int idx = hash % store->capacity;
  int start_idx = idx;

  while (true) {
    if (!store->entries[idx].occupied && !store->entries[idx].tombstone) {
      return false;
    }
    if (store->entries[idx].occupied &&
        strcmp(store->entries[idx].key, key) == 0) {
      free(store->entries[idx].key);
      kv_free_value(&store->entries[idx].value);
      store->entries[idx].occupied = false;
      store->entries[idx].tombstone = true;
      store->count--;
      return true;
    }
    idx = (idx + 1) % store->capacity;
    if (idx == start_idx)
      return false;
  }
}

// --- Typed Helpers ---

static inline void KV_SetInt(KVStore *store, const char *key, int value) {
  KVValue v = {.type = KV_INT, .as.i = value};
  KV_Set(store, key, v);
}

static inline void KV_SetFloat(KVStore *store, const char *key, float value) {
  KVValue v = {.type = KV_FLOAT, .as.f = value};
  KV_Set(store, key, v);
}

static inline void KV_SetString(KVStore *store, const char *key,
                                const char *value) {
  KVValue v = {
      .type = KV_STRING,
      .as.s = (char *)value}; // Cast for assignment, duplicated inside KV_Set
  KV_Set(store, key, v);
}

static inline void KV_SetVec2(KVStore *store, const char *key, Vector2 value) {
  KVValue v = {.type = KV_VEC2, .as.v = value};
  KV_Set(store, key, v);
}

static inline int KV_GetInt(KVStore *store, const char *key, int fallback) {
  KVValue v = KV_Get(store, key);
  return (v.type == KV_INT) ? v.as.i : fallback;
}

static inline float KV_GetFloat(KVStore *store, const char *key,
                                float fallback) {
  KVValue v = KV_Get(store, key);
  return (v.type == KV_FLOAT) ? v.as.f : fallback;
}

static inline const char *KV_GetString(KVStore *store, const char *key,
                                       const char *fallback) {
  KVValue v = KV_Get(store, key);
  return (v.type == KV_STRING) ? v.as.s : fallback;
}

static inline Vector2 KV_GetVec2(KVStore *store, const char *key,
                                 Vector2 fallback) {
  KVValue v = KV_Get(store, key);
  return (v.type == KV_VEC2) ? v.as.v : fallback;
}

// --- Iteration ---

static inline void KV_ForEach(KVStore *store,
                              void (*callback)(const char *key, KVValue val,
                                               void *ctx),
                              void *ctx) {
  if (!store)
    return;
  for (int i = 0; i < store->capacity; i++) {
    if (store->entries[i].occupied) {
      callback(store->entries[i].key, store->entries[i].value, ctx);
    }
  }
}

#endif // KVSTORE_H
