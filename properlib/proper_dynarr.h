// proper_dynarr.h
#ifndef PROPER_DYNARR_H
#define PROPER_DYNARR_H

#include <stdlib.h>
#include <string.h>

// Define a dynamic array type
#define PROPER_DYNARR_DEFINE_TYPE(Type, Suffix)                                \
  typedef struct {                                                             \
    Type *data;                                                                \
    size_t size;                                                               \
    size_t capacity;                                                           \
  } proper_dynarr_##Suffix;                                                    \
                                                                               \
  static inline void proper_dynarr_init_##Suffix(                              \
      proper_dynarr_##Suffix *arr) {                                           \
    arr->data = NULL;                                                          \
    arr->size = 0;                                                             \
    arr->capacity = 0;                                                         \
  }                                                                            \
                                                                               \
  static inline int proper_dynarr_push_##Suffix(proper_dynarr_##Suffix *arr,   \
                                                Type val) {                    \
    if (arr->size + 1 > arr->capacity) {                                       \
      size_t new_cap = arr->capacity ? arr->capacity * 2 : 4;                  \
      Type *tmp = (Type *)realloc(arr->data, new_cap * sizeof(Type));          \
      if (!tmp)                                                                \
        return -1;                                                             \
      arr->data = tmp;                                                         \
      arr->capacity = new_cap;                                                 \
    }                                                                          \
    arr->data[arr->size++] = val;                                              \
    return 0;                                                                  \
  }                                                                            \
                                                                               \
  static inline void proper_dynarr_free_##Suffix(                              \
      proper_dynarr_##Suffix *arr) {                                           \
    free(arr->data);                                                           \
    proper_dynarr_init_##Suffix(arr);                                          \
  }

#endif // PROPER_DYNARR_H

// examples
//
// PROPER_DYNARR_DEFINE_TYPE(int, int)
// PROPER_DYNARR_DEFINE_TYPE(char*, str)
//
// proper_dynarr_int arr;
// proper_dynarr_init_int(&arr);
// proper_dynarr_push_int(&arr, 42);
// proper_dynarr_free_int(&arr);
