#ifndef KVSTORE_H
#define KVSTORE_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ==================== KVStore ====================
   Simple, fast, in-memory key-value store (string → string)
   - Hash table with open addressing + linear probing
   - Automatic resizing
   - Zero dependencies
   - Thread-unsafe (add your own mutex if needed)
   Usage:
       kvstore_t *kv = kvstore_create();
       kvstore_set(kv, "name", "Alice");
       kvstore_set(kv, "age",  "30");
       printf("%s\n", kvstore_get(kv, "name"));   // Alice
       kvstore_del(kv, "age");
       kvstore_destroy(kv);
   =============================================== */

typedef struct {
  char *key;
  char *value;
  uint32_t hash;
} kvstore_entry_t;

typedef struct {
  kvstore_entry_t *entries;
  size_t capacity;
  size_t size;
} kvstore_t;

/* Internal helpers */
static inline uint32_t kvstore_hash(const char *str) {
  uint32_t hash = 5381;
  int c;
  while ((c = *str++))
    hash = ((hash << 5) + hash) + (unsigned char)c; /* hash * 33 + c */
  return hash;
}

static bool kvstore_resize(kvstore_t *kv, size_t new_capacity);

/* Public API */

static inline kvstore_t *kvstore_create(void) {
  kvstore_t *kv = calloc(1, sizeof(kvstore_t));
  if (!kv)
    return NULL;
  kv->capacity = 16;
  kv->entries = calloc(kv->capacity, sizeof(kvstore_entry_t));
  if (!kv->entries) {
    free(kv);
    return NULL;
  }
  return kv;
}

static inline void kvstore_destroy(kvstore_t *kv) {
  if (!kv)
    return;
  for (size_t i = 0; i < kv->capacity; ++i) {
    free(kv->entries[i].key);
    free(kv->entries[i].value);
  }
  free(kv->entries);
  free(kv);
}

static inline size_t kvstore_size(const kvstore_t *kv) {
  return kv ? kv->size : 0;
}

static inline void kvstore_clear(kvstore_t *kv) {
  if (!kv)
    return;
  for (size_t i = 0; i < kv->capacity; ++i) {
    if (kv->entries[i].key) {
      free(kv->entries[i].key);
      free(kv->entries[i].value);
      kv->entries[i].key = NULL;
      kv->entries[i].value = NULL;
    }
  }
  kv->size = 0;
}

/* Returns true if key existed and was overwritten */
static inline bool kvstore_set(kvstore_t *kv, const char *key,
                               const char *value) {
  if (!kv || !key || !value)
    return false;

  uint32_t hash = kvstore_hash(key);
  size_t index = (size_t)(hash & (kv->capacity - 1));

  /* Look for existing key */
  for (size_t i = 0; i < kv->capacity; ++i) {
    kvstore_entry_t *entry = &kv->entries[index];
    if (entry->key && entry->hash == hash && strcmp(entry->key, key) == 0) {
      char *new_val = strdup(value);
      if (!new_val)
        return false;
      free(entry->value);
      entry->value = new_val;
      return true; /* overwritten */
    }
    if (!entry->key)
      break; /* empty slot */
    index = (index + 1) & (kv->capacity - 1);
  }

  /* Load factor > 0.7 → resize */
  if (kv->size + 1 > kv->capacity * 7 / 10) {
    if (!kvstore_resize(kv, kv->capacity * 2))
      return false;
    index = (size_t)(hash & (kv->capacity - 1));
  }

  /* Insert new entry */
  index = (size_t)(hash & (kv->capacity - 1));
  while (kv->entries[index].key) {
    index = (index + 1) & (kv->capacity - 1);
  }

  kvstore_entry_t *entry = &kv->entries[index];
  entry->key = strdup(key);
  entry->value = strdup(value);
  entry->hash = hash;
  if (!entry->key || !entry->value) {
    free(entry->key);
    free(entry->value);
    entry->key = entry->value = NULL;
    return false;
  }
  kv->size++;
  return false; /* new key */
}

/* Returns pointer to value or NULL if not found. Do NOT free the returned
 * string! */
static inline const char *kvstore_get(const kvstore_t *kv, const char *key) {
  if (!kv || !key || kv->size == 0)
    return NULL;

  uint32_t hash = kvstore_hash(key);
  size_t index = (size_t)(hash & (kv->capacity - 1));

  for (size_t i = 0; i < kv->capacity; ++i) {
    kvstore_entry_t *entry = &kv->entries[index];
    if (!entry->key)
      return NULL; /* empty slot → not present */
    if (entry->hash == hash && strcmp(entry->key, key) == 0)
      return entry->value;
    index = (index + 1) & (kv->capacity - 1);
  }
  return NULL;
}

/* Returns true if key existed and was deleted */
static inline bool kvstore_del(kvstore_t *kv, const char *key) {
  if (!kv || !key || kv->size == 0)
    return false;

  uint32_t hash = kvstore_hash(key);
  size_t index = (size_t)(hash & (kv->capacity - 1));

  for (size_t i = 0; i < kv->capacity; ++i) {
    kvstore_entry_t *entry = &kv->entries[index];
    if (!entry->key)
      return false;
    if (entry->hash == hash && strcmp(entry->key, key) == 0) {
      free(entry->key);
      free(entry->value);
      entry->key = entry->value = NULL;

      /* Rehash all following entries (linear probing needs this) */
      size_t next = (index + 1) & (kv->capacity - 1);
      while (kv->entries[next].key) {
        kvstore_entry_t tmp = kv->entries[next];
        kv->entries[next] = (kvstore_entry_t){0};
        kv->size--;
        kvstore_set(kv, tmp.key, tmp.value); /* will re-insert correctly */
        free(tmp.key);
        free(tmp.value);
        next = (next + 1) & (kv->capacity - 1);
      }
      kv->size--;
      return true;
    }
    index = (index + 1) & (kv->capacity - 1);
  }
  return false;
}

/* Simple iterator */
typedef struct {
  const kvstore_t *kv;
  size_t index;
} kvstore_iter_t;

static inline kvstore_iter_t kvstore_iter(const kvstore_t *kv) {
  return (kvstore_iter_t){.kv = kv, .index = 0};
}

static inline bool kvstore_iter_next(kvstore_iter_t *iter, const char **key,
                                     const char **value) {
  if (!iter || !iter->kv)
    return false;
  while (iter->index < iter->kv->capacity) {
    if (iter->kv->entries[iter->index].key) {
      if (key)
        *key = iter->kv->entries[iter->index].key;
      if (value)
        *value = iter->kv->entries[iter->index].value;
      iter->index++;
      return true;
    }
    iter->index++;
  }
  return false;
}

/* Internal resize (power-of-two only) */
static bool kvstore_resize(kvstore_t *kv, size_t new_capacity) {
  if (new_capacity < 16)
    new_capacity = 16;
  size_t old_capacity = kv->capacity;
  kvstore_entry_t *old_entries = kv->entries;

  kv->capacity = new_capacity;
  kv->size = 0;
  kv->entries = calloc(kv->capacity, sizeof(kvstore_entry_t));
  if (!kv->entries) {
    kv->entries = old_entries;
    kv->capacity = old_capacity;
    return false;
  }

  for (size_t i = 0; i < old_capacity; ++i) {
    if (old_entries[i].key) {
      kvstore_set(kv, old_entries[i].key, old_entries[i].value);
      free(old_entries[i].key);
      free(old_entries[i].value);
    }
  }
  free(old_entries);
  return true;
}

#endif /* KVSTORE_H */
