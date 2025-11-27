#ifndef C_VEC_H
#define C_VEC_H

#include <stdlib.h> // For malloc, realloc, free
#include <string.h> // For memcpy

// --- Internal Structure Definitions ---

// Metadata structure stored *before* the user's data pointer
typedef struct {
    size_t count;
    size_t capacity;
    size_t stride; // Size of a single element (sizeof(T))
} c_vec_hdr_t;

// Macro to get the header structure from the user's data pointer
#define C_VEC_HDR(v) ((c_vec_hdr_t *)((char *)(v) - sizeof(c_vec_hdr_t)))

// --- Core API Macros ---

// Get the current number of elements
#define c_vec_count(v)          ((v) ? C_VEC_HDR(v)->count : 0)

// Get the current capacity
#define c_vec_capacity(v)       ((v) ? C_VEC_HDR(v)->capacity : 0)

// Free the vector memory
#define c_vec_free(v)           ((v) ? (free(C_VEC_HDR(v)), (v) = NULL) : (void)0)

// --- Internal Implementation Functions ---

// Function to grow the vector and update metadata
void *c_vec_grow(void *v, size_t new_cap, size_t elem_size);

// Macro to ensure enough space for one more element, growing if necessary
#define c_vec_maybe_grow(v, elem_size) \
    ((!(v) || C_VEC_HDR(v)->count >= C_VEC_HDR(v)->capacity) \
        ? (v) = c_vec_grow((v), (v) ? C_VEC_HDR(v)->capacity * 2 : 8, elem_size) : (v))

// --- Primary API ---

// Push an element onto the end of the vector (requires a temporary variable for the item)
#define c_vec_push(v, item) \
    (c_vec_maybe_grow((v), sizeof(item)), \
     (v)[C_VEC_HDR(v)->count++] = (item))

// Remove the last element and return a pointer to it (must be casted)
#define c_vec_pop(v) \
    ((v) && C_VEC_HDR(v)->count > 0 ? &(v)[--C_VEC_HDR(v)->count] : NULL)


// --- Implementation Section ---
#ifdef C_VEC_IMPLEMENTATION

void *c_vec_grow(void *v, size_t new_cap, size_t elem_size) {
    // If v is NULL (initial allocation)
    if (!v) {
        new_cap = new_cap ? new_cap : 8; // Default initial capacity
        size_t total_size = sizeof(c_vec_hdr_t) + new_cap * elem_size;
        
        c_vec_hdr_t *hdr = (c_vec_hdr_t*)malloc(total_size);
        if (!hdr) return NULL; // Allocation failed

        hdr->count = 0;
        hdr->capacity = new_cap;
        hdr->stride = elem_size;
        
        // Return the pointer *after* the header
        return (void *)((char *)hdr + sizeof(c_vec_hdr_t));
    }
    
    // If v is not NULL (reallocation)
    c_vec_hdr_t *old_hdr = C_VEC_HDR(v);
    size_t new_total_size = sizeof(c_vec_hdr_t) + new_cap * old_hdr->stride;
    
    c_vec_hdr_t *new_hdr = (c_vec_hdr_t*)realloc(old_hdr, new_total_size);
    if (!new_hdr) return NULL; // Reallocation failed
    
    new_hdr->capacity = new_cap;
    
    // Return the new pointer *after* the header
    return (void *)((char *)new_hdr + sizeof(c_vec_hdr_t));
}

#endif // C_VEC_IMPLEMENTATION

#endif // C_VEC_H
