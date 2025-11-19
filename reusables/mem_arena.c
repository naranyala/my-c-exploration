typedef struct {
  char *base;
  size_t used;
  size_t capacity;
} Arena;

void arena_init(Arena *a, size_t cap) {
  a->base = malloc(cap);
  a->capacity = cap;
  a->used = 0;
}

void *arena_alloc(Arena *a, size_t size) {
  if (a->used + size > a->capacity)
    abort(); // or grow
  void *p = a->base + a->used;
  a->used += (size + 7) & ~7; // align to 8
  return p;
}

#define NEW(a, type) (type *)arena_alloc(a, sizeof(type))
#define NEW_ARRAY(a, type, n) (type *)arena_alloc(a, sizeof(type) * (n))
