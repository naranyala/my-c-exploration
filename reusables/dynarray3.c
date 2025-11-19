#include <stdlib.h>
#include <string.h>

typedef struct {
  void *data;
  size_t size;
  size_t capacity;
  size_t elem_size;
} Vector;

#define vector_init(v, type) _vector_init(v, sizeof(type))
#define vector_push(v, val)                                                    \
  do {                                                                         \
    if ((v)->size == (v)->capacity)                                            \
      _vector_grow(v);                                                         \
    (v)->data = memcpy((char *)(v)->data + (v)->size * (v)->elem_size, &(val), \
                       (v)->elem_size);                                        \
    (v)->size++;                                                               \
  } while (0)

static void _vector_init(Vector *v, size_t elem_size) {
  v->capacity = 8;
  v->size = 0;
  v->elem_size = elem_size;
  v->data = malloc(v->capacity * elem_size);
}

static void _vector_grow(Vector *v) {
  v->capacity *= 2;
  v->data = realloc(v->data, v->capacity * v->elem_size);
}
