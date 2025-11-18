#ifndef SLICES_H
#define SLICES_H
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

/*
 * Slice utility - views into contiguous data without ownership
 *
 * Usage:
 * int arr[] = {1, 2, 3, 4, 5};
 * slice(int) s = slice_from_array(arr, 5);
 * slice(int) sub = slice_slice(s, 1, 3); // {2, 3}
 */

// Main slice type
#define slice(T) struct { T *ptr; size_t len; }

// Comparison function type
typedef int (*slice_cmp_fn)(const void *, const void *);

// Construction
#define slice_from_array(arr, count) \
    (slice(typeof(*(arr)))){ (arr), (count) }
#define slice_from_ptr(ptr, count) \
    (slice(typeof(*(ptr)))){ (ptr), (count) }
#define slice_empty(T) \
    (slice(T)){ NULL, 0 }

// Accessors
#define slice_len(s) ((s).len)
#define slice_ptr(s) ((s).ptr)
#define slice_empty(s) ((s).len == 0 || (s).ptr == NULL)
#define slice_first(s, default_val) \
    (slice_empty(s) ? (default_val) : (s).ptr[0])
#define slice_last(s, default_val) \
    (slice_empty(s) ? (default_val) : (s).ptr[(s).len - 1])

// Element access with bounds checking
#define slice_get(s, i, default_val) \
    ((i) < (s).len ? (s).ptr[(i)] : (default_val))
#define slice_try_get(s, i, out_ptr) \
    ((i) < (s).len ? (*(out_ptr) = (s).ptr[(i)], true) : false)

// Slicing operations
#define slice_slice(s, start, end) \
    (typeof(s)){ \
        (s).ptr + ((start) <= (s).len ? (start) : (s).len), \
        ((start) <= (s).len && (end) >= (start) && (end) <= (s).len) ? (end) - (start) : \
        ((start) <= (s).len ? (s).len - (start) : 0) \
    }
#define slice_sub(s, start, count) \
    (typeof(s)){ \
        (s).ptr + ((start) <= (s).len ? (start) : (s).len), \
        ((start) <= (s).len && (count) <= (s).len - (start) ? (count) : (s).len - (start)) \
    }
#define slice_from_start(s, count) slice_sub((s), 0, (count))
#define slice_from_end(s, count) slice_sub((s), (s).len - (count), (count))
#define slice_drop_start(s, count) slice_sub((s), (count), (s).len - (count))
#define slice_drop_end(s, count) slice_sub((s), 0, (s).len - (count))

// Comparison
#define slice_equal(a, b, cmp_fn) \
    ({ \
        bool _equal = (a).len == (b).len; \
        if (_equal && (a).ptr != (b).ptr) { \
            if (cmp_fn) { \
                for (size_t _i = 0; _i < (a).len; _i++) { \
                    if (cmp_fn(&(a).ptr[_i], &(b).ptr[_i]) != 0) { \
                        _equal = false; \
                        break; \
                    } \
                } \
            } else { \
                _equal = memcmp((a).ptr, (b).ptr, (a).len * sizeof(*(a).ptr)) == 0; \
            } \
        } \
        _equal; \
    })
#define slice_compare(a, b, cmp_fn) \
    ({ \
        int _cmp = (a).len < (b).len ? -1 : (a).len > (b).len ? 1 : 0; \
        if (_cmp == 0 && (a).ptr != (b).ptr) { \
            if (cmp_fn) { \
                for (size_t _i = 0; _i < (a).len; _i++) { \
                    _cmp = cmp_fn(&(a).ptr[_i], &(b).ptr[_i]); \
                    if (_cmp != 0) break; \
                } \
            } else { \
                _cmp = memcmp((a).ptr, (b).ptr, (a).len * sizeof(*(a).ptr)); \
            } \
        } \
        _cmp; \
    })

// Searching
#define slice_find(s, value, cmp_fn) \
    ({ \
        size_t _idx = (s).len; \
        for (size_t _i = 0; _i < (s).len; _i++) { \
            if (cmp_fn ? cmp_fn(&(s).ptr[_i], &(value)) == 0 : \
                (s).ptr[_i] == (value)) { \
                _idx = _i; \
                break; \
            } \
        } \
        _idx; \
    })
#define slice_contains(s, value, cmp_fn) \
    (slice_find((s), (value), (cmp_fn)) < (s).len)

// Iteration
#define slice_foreach(s, iter) \
    for (typeof((s).ptr) iter = (s).ptr; \
         !slice_empty(s) && iter < (s).ptr + (s).len; \
         iter++)
#define slice_foreach_idx(s, iter, idx) \
    for (typeof((s).ptr) iter = (s).ptr, idx = 0; \
         !slice_empty(s) && idx < (s).len; \
         iter++, idx++)

// String slices (common special case)
typedef slice(char) str_slice;
#define str_slice_from_string(s) \
    (str_slice){ (s), strlen(s) }
#define str_slice_from_literal(s) \
    (str_slice){ (s), sizeof(s) - 1 }
#define str_slice_equal(a, b) \
    ((a).len == (b).len && memcmp((a).ptr, (b).ptr, (a).len) == 0)
#define str_slice_compare(a, b) \
    strncmp((a).ptr, (b).ptr, (a).len < (b).len ? (a).len : (b).len)
#define str_slice_to_cstr(s, buf, buf_size) \
    ({ \
        size_t _copy_len = (s).len < (buf_size) - 1 ? (s).len : (buf_size) - 1; \
        memcpy((buf), (s).ptr, _copy_len); \
        (buf)[_copy_len] = '\0'; \
        _copy_len; \
    })

// Allocates a new C string from a str_slice. Returns NULL on allocation failure.
#define str_slice_dup(s) \
    ({ \
        char *_str = malloc((s).len + 1); \
        if (_str) { \
            memcpy(_str, (s).ptr, (s).len); \
            _str[(s).len] = '\0'; \
        } \
        _str; \
    })

#endif /* SLICES_H */
