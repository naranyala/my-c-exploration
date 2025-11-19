typedef struct {
  char *key;
  void *value;
  size_t value_size;
} HashEntry;

typedef struct {
  HashEntry *entries;
  size_t capacity;
  size_t size;
} HashMap;

void hm_init(HashMap *hm, size_t initial_cap) {
  hm->capacity = initial_cap;
  hm->size = 0;
  hm->entries = calloc(initial_cap, sizeof(HashEntry));
}

static uint32_t hash(const char *s) {
  uint32_t h = 5381;
  int c;
  while ((c = *s++))
    h = ((h << 5) + h) + c;
  return h;
}

// Insert or update
void hm_set(HashMap *hm, const char *key, void *value, size_t value_size) {
  if (hm->size >= hm->capacity * 0.7) {
    // grow logic omitted for brevity (double capacity + rehash)
  }
  uint32_t idx = hash(key) % hm->capacity;
  while (hm->entries[idx].key && strcmp(hm->entries[idx].key, key) != 0) {
    idx = (idx + 1) % hm->capacity;
  }
  if (!hm->entries[idx].key)
    hm->size++;
  hm->entries[idx].key = strdup(key);
  hm->entries[idx].value = memcpy(malloc(value_size), value, value_size);
  hm->entries[idx].value_size = value_size;
}

void *hm_get(HashMap *hm, const char *key) { /* similar probing */ }
