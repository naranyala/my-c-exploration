
// dynarray.h
#ifndef DYNARRAY_H
#define DYNARRAY_H

#include <stdlib.h>

typedef struct {
  void **items;
  size_t size;
  size_t capacity;
} DynArray;

static inline DynArray *dynarray_new(size_t capacity) {
  DynArray *arr = malloc(sizeof(DynArray));
  arr->items = malloc(sizeof(void *) * capacity);
  arr->size = 0;
  arr->capacity = capacity;
  return arr;
}

static inline void dynarray_push(DynArray *arr, void *item) {
  if (arr->size == arr->capacity) {
    arr->capacity *= 2;
    arr->items = realloc(arr->items, sizeof(void *) * arr->capacity);
  }
  arr->items[arr->size++] = item;
}

static inline void dynarray_free(DynArray *arr) {
  free(arr->items);
  free(arr);
}

#endif
