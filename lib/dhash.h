/* dhash.h -- Damn-Stupid Hash Table for C
 * MIT License
 * Usage: #define DHASH_IMPL before including in one .c file
 */

#ifndef DHASH_H
#define DHASH_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <string.h>

// ----------------------------
// CONFIGURATION
// ----------------------------

#ifndef DHASH_TABLE_SIZE
// Must be power of two! Default: 16 slots
#define DHASH_TABLE_SIZE 16
#endif

#ifndef DHASH_KEY_MAX_LEN
#define DHASH_KEY_MAX_LEN 32
#endif

// ----------------------------
// PUBLIC API
// ----------------------------

// Store a key-value pair (copies key, stores void* value)
// Returns 0 on success, -1 if out of space or key too long
int dhash_set(const char *key, void *value);

// Retrieve value by key
// Returns value, or NULL if not found
void *dhash_get(const char *key);

// Remove a key
// Returns 0 if removed, -1 if not found
int dhash_del(const char *key);

// Clear entire table
void dhash_clear(void);

#ifdef __cplusplus
}
#endif

#endif // DHASH_H

// ----------------------------
// IMPLEMENTATION
// ----------------------------

#ifdef DHASH_IMPL

#if (DHASH_TABLE_SIZE & (DHASH_TABLE_SIZE - 1)) != 0
#error DHASH_TABLE_SIZE must be a power of two!
#endif

typedef struct {
  char key[DHASH_KEY_MAX_LEN];
  void *value;
  int occupied;
} dhash_entry_t;

static dhash_entry_t _table[DHASH_TABLE_SIZE] = {0};

// FNV-1a hash (simplified for small strings)
static unsigned int _dhash_hash(const char *str) {
  unsigned int hash = 2166136261u;
  while (*str) {
    hash ^= (unsigned char)(*str++);
    hash *= 16777619;
  }
  return hash;
}

int dhash_set(const char *key, void *value) {
  if (!key || !value)
    return -1;
  size_t len = strlen(key);
  if (len >= DHASH_KEY_MAX_LEN)
    return -1;

  unsigned int idx = _dhash_hash(key) & (DHASH_TABLE_SIZE - 1);

  // Linear probing
  for (size_t i = 0; i < DHASH_TABLE_SIZE; ++i) {
    dhash_entry_t *entry = &_table[idx];
    if (!entry->occupied) {
      // Found empty slot
      strncpy(entry->key, key, DHASH_KEY_MAX_LEN - 1);
      entry->key[DHASH_KEY_MAX_LEN - 1] = '\0';
      entry->value = value;
      entry->occupied = 1;
      return 0;
    }
    if (strcmp(entry->key, key) == 0) {
      // Key exists — update value
      entry->value = value;
      return 0;
    }
    idx = (idx + 1) & (DHASH_TABLE_SIZE - 1); // wrap around
  }

  return -1; // table full
}

void *dhash_get(const char *key) {
  if (!key)
    return NULL;

  unsigned int idx = _dhash_hash(key) & (DHASH_TABLE_SIZE - 1);

  for (size_t i = 0; i < DHASH_TABLE_SIZE; ++i) {
    dhash_entry_t *entry = &_table[idx];
    if (!entry->occupied)
      break;
    if (strcmp(entry->key, key) == 0) {
      return entry->value;
    }
    idx = (idx + 1) & (DHASH_TABLE_SIZE - 1);
  }

  return NULL;
}

int dhash_del(const char *key) {
  if (!key)
    return -1;

  unsigned int idx = _dhash_hash(key) & (DHASH_TABLE_SIZE - 1);

  for (size_t i = 0; i < DHASH_TABLE_SIZE; ++i) {
    dhash_entry_t *entry = &_table[idx];
    if (!entry->occupied)
      break;
    if (strcmp(entry->key, key) == 0) {
      entry->occupied = 0;
      return 0;
    }
    idx = (idx + 1) & (DHASH_TABLE_SIZE - 1);
  }

  return -1;
}

void dhash_clear(void) {
  for (size_t i = 0; i < DHASH_TABLE_SIZE; ++i) {
    _table[i].occupied = 0;
  }
}

#endif // DHASH_IMPL
