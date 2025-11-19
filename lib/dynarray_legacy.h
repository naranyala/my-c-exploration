#ifndef DYNARRAY_H
#define DYNARRAY_H

#include <stdlib.h>
#include <string.h>

/*
 * Dynamic array utility - type-safe generic arrays
 *
 * Usage:
 *   dynarray(int) numbers = {0};
 *   dynarray_append(&numbers, 42);
 *   dynarray_free(&numbers);
 */

#define dynarray(T)                                                            \
  struct {                                                                     \
    T *data;                                                                   \
    size_t len, cap;                                                           \
  }

// Internal growth helper
#define _dynarray_grow(arr, need)                                              \
  do {                                                                         \
    if ((arr)->len + (need) >= (arr)->cap) {                                   \
      size_t new_cap = (arr)->cap ? (arr)->cap * 2 : 8;                        \
      while (new_cap < (arr)->len + (need))                                    \
        new_cap *= 2;                                                          \
      (arr)->data = realloc((arr)->data, new_cap * sizeof(*(arr)->data));      \
      (arr)->cap = new_cap;                                                    \
    }                                                                          \
  } while (0)

// Basic operations
#define dynarray_append(arr, value)                                            \
  do {                                                                         \
    _dynarray_grow((arr), 1);                                                  \
    (arr)->data[(arr)->len++] = (value);                                       \
  } while (0)

#define dynarray_pop(arr)                                                      \
  ((arr)->len > 0 ? (arr)->data[--(arr)->len] : (typeof(*(arr)->data)){0})

#define dynarray_clear(arr) ((arr)->len = 0)

#define dynarray_free(arr)                                                     \
  do {                                                                         \
    free((arr)->data);                                                         \
    (arr)->data = NULL;                                                        \
    (arr)->len = (arr)->cap = 0;                                               \
  } while (0)

// Accessors
#define dynarray_get(arr, i) ((arr)->data[(i)])
#define dynarray_first(arr)                                                    \
  ((arr)->len > 0 ? (arr)->data[0] : (typeof(*(arr)->data)){0})
#define dynarray_last(arr)                                                     \
  ((arr)->len > 0 ? (arr)->data[(arr)->len - 1] : (typeof(*(arr)->data)){0})
#define dynarray_len(arr) ((arr)->len)
#define dynarray_empty(arr) ((arr)->len == 0)

// Insertion/removal
#define dynarray_insert(arr, idx, value)                                       \
  do {                                                                         \
    if ((idx) <= (arr)->len) {                                                 \
      _dynarray_grow((arr), 1);                                                \
      memmove((arr)->data + (idx) + 1, (arr)->data + (idx),                    \
              ((arr)->len - (idx)) * sizeof(*(arr)->data));                    \
      (arr)->data[(idx)] = (value);                                            \
      (arr)->len++;                                                            \
    }                                                                          \
  } while (0)

#define dynarray_remove(arr, idx)                                              \
  do {                                                                         \
    if ((idx) < (arr)->len) {                                                  \
      memmove((arr)->data + (idx), (arr)->data + (idx) + 1,                    \
              ((arr)->len - (idx) - 1) * sizeof(*(arr)->data));                \
      (arr)->len--;                                                            \
    }                                                                          \
  } while (0)

// Bulk operations
#define dynarray_append_array(arr, src_array, count)                           \
  do {                                                                         \
    size_t _count = (count);                                                   \
    _dynarray_grow((arr), _count);                                             \
    memcpy((arr)->data + (arr)->len, (src_array),                              \
           _count * sizeof(*(arr)->data));                                     \
    (arr)->len += _count;                                                      \
  } while (0)

#define dynarray_reserve(arr, capacity)                                        \
  do {                                                                         \
    if ((capacity) > (arr)->cap) {                                             \
      (arr)->data = realloc((arr)->data, (capacity) * sizeof(*(arr)->data));   \
      (arr)->cap = (capacity);                                                 \
    }                                                                          \
  } while (0)

// Iteration
#define dynarray_foreach(arr, iter)                                            \
  for (typeof((arr)->data) iter = (arr)->data;                                 \
       iter < (arr)->data + (arr)->len; iter++)

#endif /* DYNARRAY_H */
