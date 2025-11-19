/*
 * dynarray.h — Modern single-header dynamic array for C (C99+)
 *
 * Updated: declare pointer variable inside da_foreach_ptr so users don't need
 * to pre-declare it (fixes "undeclared" errors).
 *
 * See previous version for full history — only da_foreach_ptr changed.
 */

#ifndef DYNARRAY_H
#define DYNARRAY_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/* Configuration */
#ifndef DA_INIT_CAPACITY
#define DA_INIT_CAPACITY 8
#endif

#ifndef DA_GROWTH_FACTOR
#define DA_GROWTH_FACTOR 2
#endif

/* Optional: custom allocators */
#ifndef DA_MALLOC
#define DA_MALLOC malloc
#endif

#ifndef DA_REALLOC
#define DA_REALLOC realloc
#endif

#ifndef DA_FREE
#define DA_FREE free
#endif

/* Optional: OOM handler (string describing failed op) */
#ifndef DA_ON_OOM
#define DA_ON_OOM(msg) abort()
#endif

/* Public type: dynamic array of Type is da_t(Type) */
#define da_t(Type) Type *

/* Internal header stored immediately before the array pointer */
typedef struct {
  size_t count;
  size_t capacity;
} da_hdr_t;

/* Internal helpers */
static inline da_hdr_t *da__hdr(const void *arr) {
  return arr ? ((da_hdr_t *)arr) - 1 : NULL;
}

static inline void *da__grow(void *arr, size_t min_cap, size_t item_size) {
  da_hdr_t *old_hdr = da__hdr(arr);
  size_t old_cap = old_hdr ? old_hdr->capacity : 0;
  size_t new_cap = old_cap ? old_cap * DA_GROWTH_FACTOR : DA_INIT_CAPACITY;
  if (new_cap < min_cap)
    new_cap = min_cap;

  size_t total = sizeof(da_hdr_t) + new_cap * item_size;
  da_hdr_t *new_hdr;

  if (old_hdr) {
    new_hdr = (da_hdr_t *)DA_REALLOC(old_hdr, total);
  } else {
    new_hdr = (da_hdr_t *)DA_MALLOC(total);
  }

  if (!new_hdr) {
    DA_ON_OOM("dynarray allocation failed");
    return arr; /* return original pointer (unchanged) */
  }

  if (!old_hdr)
    new_hdr->count = 0;
  new_hdr->capacity = new_cap;
  return (void *)(new_hdr + 1);
}

/* Query macros */
#define da_count(a) ((a) ? da__hdr(a)->count : 0)
#define da_capacity(a) ((a) ? da__hdr(a)->capacity : 0)
#define da_empty(a) (da_count(a) == 0)
#define da_size(a) da_count(a) /* alias */

/* Lifecycle */
#define da_free(a)                                                             \
  do {                                                                         \
    if (a) {                                                                   \
      DA_FREE(da__hdr(a));                                                     \
      (a) = NULL;                                                              \
    }                                                                          \
  } while (0)

#define da_clear(a)                                                            \
  do {                                                                         \
    if (a)                                                                     \
      da__hdr(a)->count = 0;                                                   \
  } while (0)

/* Reserve capacity (does not change count) */
#define da_reserve(a, cap)                                                     \
  do {                                                                         \
    size_t __cap = (size_t)(cap);                                              \
    if (__cap > da_capacity(a)) {                                              \
      (a) = da__grow((a), __cap, sizeof *(a));                                 \
    }                                                                          \
  } while (0)

/* Resize array (changes count). New elements zero-initialized. */
#define da_resize(a, new_count)                                                \
  do {                                                                         \
    size_t __new_count = (size_t)(new_count);                                  \
    da_reserve(a, __new_count);                                                \
    if (__new_count > da_count(a)) {                                           \
      memset((a) + da_count(a), 0, (__new_count - da_count(a)) * sizeof *(a)); \
    }                                                                          \
    if (a)                                                                     \
      da__hdr(a)->count = __new_count;                                         \
  } while (0)

/* Shrink capacity to fit current count */
#define da_shrink(a)                                                           \
  do {                                                                         \
    if ((a) && da_count(a) < da_capacity(a)) {                                 \
      size_t __size = sizeof(da_hdr_t) + da_count(a) * sizeof *(a);            \
      da_hdr_t *__h = (da_hdr_t *)DA_REALLOC(da__hdr(a), __size);              \
      if (__h) {                                                               \
        __h->capacity = __h->count;                                            \
        (a) = (void *)(__h + 1);                                               \
      }                                                                        \
    }                                                                          \
  } while (0)

/* ===================== insertion / mutation ===================== */

/* da_push: variadic so expressions containing commas (e.g. compound literals)
 * work without extra parentheses. Requires C99 variadic macros.
 *
 * Example:
 *   da_push(arr, (point){ .x = 1, .y = 2 });
 */
#define da_push(a, ...)                                                        \
  do {                                                                         \
    if (!(a) || da_count(a) >= da_capacity(a)) {                               \
      (a) = da__grow((a), da_count(a) + 1, sizeof *(a));                       \
    }                                                                          \
    (a)[da__hdr(a)->count++] = (__VA_ARGS__);                                  \
  } while (0)

/* Push many from pointer `items`, count `n` */
#define da_push_many(a, items, n)                                              \
  do {                                                                         \
    size_t __n = (size_t)(n);                                                  \
    if (__n) {                                                                 \
      size_t __new_count = da_count(a) + __n;                                  \
      if (__new_count > da_capacity(a)) {                                      \
        (a) = da__grow((a), __new_count, sizeof *(a));                         \
      }                                                                        \
      memcpy((a) + da_count(a), (items), __n * sizeof *(a));                   \
      da__hdr(a)->count = __new_count;                                         \
    }                                                                          \
  } while (0)

/* Insert at index, shifts elements right. If idx == count, acts like push. */
#define da_insert(a, idx, item)                                                \
  do {                                                                         \
    size_t __idx = (size_t)(idx);                                              \
    if (__idx <= da_count(a)) {                                                \
      if (da_count(a) >= da_capacity(a)) {                                     \
        (a) = da__grow((a), da_count(a) + 1, sizeof *(a));                     \
      }                                                                        \
      if (__idx < da_count(a)) {                                               \
        memmove((a) + __idx + 1, (a) + __idx,                                  \
                (da_count(a) - __idx) * sizeof *(a));                          \
      }                                                                        \
      (a)[__idx] = (item);                                                     \
      da__hdr(a)->count++;                                                     \
    }                                                                          \
  } while (0)

/* Pop last element and return it: use as an expression. If empty, behavior:
 * - evaluates to (a)[0] (undefined if a==NULL); use da_count check when needed.
 * This mirrors many small C libraries; change if you want an assert/abort.
 */
#define da_pop(a) ((a) && da_count(a) > 0 ? (a)[--da__hdr(a)->count] : (a)[0])

/* Remove at index, preserving order (O(n)) */
#define da_remove(a, idx)                                                      \
  do {                                                                         \
    size_t __idx = (size_t)(idx);                                              \
    if ((a) && __idx < da_count(a)) {                                          \
      if (__idx < da_count(a) - 1) {                                           \
        memmove((a) + __idx, (a) + __idx + 1,                                  \
                (da_count(a) - __idx - 1) * sizeof *(a));                      \
      }                                                                        \
      da__hdr(a)->count--;                                                     \
    }                                                                          \
  } while (0)

/* Unordered remove: swap with last (O(1)) */
#define da_remove_fast(a, idx)                                                 \
  do {                                                                         \
    size_t __idx = (size_t)(idx);                                              \
    if ((a) && __idx < da_count(a)) {                                          \
      (a)[__idx] = (a)[da__hdr(a)->count - 1];                                 \
      da__hdr(a)->count--;                                                     \
    }                                                                          \
  } while (0)

/* Remove first matching item (==). Sets found (bool) to true if removed. */
#define da_remove_item(a, item, found)                                         \
  do {                                                                         \
    (found) = false;                                                           \
    if (a) {                                                                   \
      for (size_t __i = 0; __i < da_count(a); ++__i) {                         \
        if ((a)[__i] == (item)) {                                              \
          da_remove(a, __i);                                                   \
          (found) = true;                                                      \
          break;                                                               \
        }                                                                      \
      }                                                                        \
    }                                                                          \
  } while (0)

/* ========================= access & iteration ========================= */

#define da_get(a, idx) ((size_t)(idx) < da_count(a) ? (a)[(idx)] : (a)[0])
#define da_set(a, idx, val)                                                    \
  do {                                                                         \
    if ((size_t)(idx) < da_count(a)) {                                         \
      (a)[(idx)] = (val);                                                      \
    }                                                                          \
  } while (0)

#define da_first(a) ((a)[0])
#define da_last(a) ((a)[da_count(a) - 1])
#define da_at(a, idx) (&(a)[(idx)])

/* Iterate value copy (supports break/continue/return) */
#define da_foreach(Type, var, arr)                                             \
  for (size_t __da_i = 0, __da_keep = 1; __da_i < da_count(arr) && __da_keep;  \
       __da_keep = 0)                                                          \
    for (Type var = (arr)[__da_i]; __da_keep; __da_keep = 0, ++__da_i)

/* Iterate pointer (modify elements)
 *
 * Implementation note: the macro now declares the pointer variable (Type *ptr)
 * for you. This avoids "undeclared" errors in call sites like:
 *   da_foreach_ptr(point, pptr, pts) { ... }
 *
 * The inner for constructs a single-iteration loop that creates `ptr` and
 * then sets it to NULL to exit the inner loop; the outer loop advances the
 * index.
 */
#define da_foreach_ptr(Type, ptr, arr)                                         \
  for (size_t __da_i2 = 0; __da_i2 < da_count(arr); ++__da_i2)                 \
    for (Type *ptr = &(arr)[__da_i2]; ptr; ptr = NULL)

/* Reverse iterate */
#define da_foreach_reverse(Type, var, arr)                                     \
  for (size_t __da_i3 = da_count(arr), __da_keep3 = 1;                         \
       __da_i3 > 0 && __da_keep3; __da_keep3 = 0)                              \
    for (Type var = (arr)[--__da_i3]; __da_keep3; __da_keep3 = 0)

/* ========================= utilities ========================= */

#define da_reverse(a)                                                          \
  do {                                                                         \
    if ((a) && da_count(a) > 1) {                                              \
      for (size_t __i = 0, __n = da_count(a); __i < __n / 2; ++__i) {          \
        size_t __j = __n - __i - 1;                                            \
        /* swap via temporary buffer */                                        \
        unsigned char __buf[256];                                              \
        if (sizeof *(a) <= sizeof __buf) {                                     \
          memcpy(__buf, &(a)[__i], sizeof *(a));                               \
          memcpy(&(a)[__i], &(a)[__j], sizeof *(a));                           \
          memcpy(&(a)[__j], __buf, sizeof *(a));                               \
        } else {                                                               \
          size_t __es = sizeof *(a);                                           \
          unsigned char *__t = (unsigned char *)malloc(__es);                  \
          if (__t) {                                                           \
            memcpy(__t, &(a)[__i], __es);                                      \
            memcpy(&(a)[__i], &(a)[__j], __es);                                \
            memcpy(&(a)[__j], __t, __es);                                      \
            free(__t);                                                         \
          }                                                                    \
        }                                                                      \
      }                                                                        \
    }                                                                          \
  } while (0)

/* Clone (shallow copy of elements) */
#define da_clone(dest, src)                                                    \
  do {                                                                         \
    (dest) = NULL;                                                             \
    if (src) {                                                                 \
      da_reserve(dest, da_count(src));                                         \
      memcpy((dest), (src), da_count(src) * sizeof *(src));                    \
      da__hdr(dest)->count = da_count(src);                                    \
    }                                                                          \
  } while (0)

/* contains / find */
#define da_contains(a, item, result)                                           \
  do {                                                                         \
    (result) = false;                                                          \
    if (a) {                                                                   \
      for (size_t __i = 0; __i < da_count(a); ++__i) {                         \
        if ((a)[__i] == (item)) {                                              \
          (result) = true;                                                     \
          break;                                                               \
        }                                                                      \
      }                                                                        \
    }                                                                          \
  } while (0)

#define da_find(a, item, index)                                                \
  do {                                                                         \
    (index) = -1;                                                              \
    if (a) {                                                                   \
      for (size_t __i = 0; __i < da_count(a); ++__i) {                         \
        if ((a)[__i] == (item)) {                                              \
          (index) = (int)__i;                                                  \
          break;                                                               \
        }                                                                      \
      }                                                                        \
    }                                                                          \
  } while (0)

#endif /* DYNARRAY_H */
