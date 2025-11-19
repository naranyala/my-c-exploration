
// hashmap.h
#ifndef HASHMAP_H
#define HASHMAP_H

#include <stdlib.h>
#include <string.h>

#define HASH_SIZE 128

typedef struct Entry {
  char *key;
  void *value;
  struct Entry *next;
} Entry;

typedef struct {
  Entry *table[HASH_SIZE];
} HashMap;

static inline unsigned hash(const char *s) {
  unsigned h = 0;
  while (*s)
    h = (h << 5) + *s++;
  return h % HASH_SIZE;
}

static inline void hashmap_put(HashMap *map, const char *key, void *value) {
  unsigned h = hash(key);
  Entry *e = malloc(sizeof(Entry));
  e->key = strdup(key);
  e->value = value;
  e->next = map->table[h];
  map->table[h] = e;
}

static inline void *hashmap_get(HashMap *map, const char *key) {
  for (Entry *e = map->table[hash(key)]; e; e = e->next)
    if (strcmp(e->key, key) == 0)
      return e->value;
  return NULL;
}

#endif
