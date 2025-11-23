/* ergo_vec.h - Type-Safe Dynamic Array/Vector */
#ifndef ERGO_VEC_H
#define ERGO_VEC_H

#include <stdlib.h>
#include <string.h>

// Internal struct stored before the user's array pointer
typedef struct {
    size_t capacity;
    size_t size;
} ergo_vec_header;

// Macro to get header from a data pointer
#define VEC_HDR(v)      ((ergo_vec_header*)((char*)(v) - sizeof(ergo_vec_header)))

// --- Core API Macros (Type-Safe) ---

// Returns the current size (number of elements)
#define ergo_vec_len(v)     ((v) ? VEC_HDR(v)->size : 0)

// Pushes an item to the end, resizing if necessary
#define ergo_vec_push(v, item) do { \
    if ((v) && VEC_HDR(v)->size == VEC_HDR(v)->capacity) { \
        (v) = ergo_vec_grow(v, sizeof(*(v))); \
    } \
    if (v) { \
        (v)[VEC_HDR(v)->size++] = (item); \
    } \
} while(0)

// Frees the entire vector (data + header)
#define ergo_vec_free(v) do { \
    if (v) { \
        free(VEC_HDR(v)); \
    } \
} while(0)

// Internal function to handle the complex resizing logic
void* ergo_vec_grow(void* v, size_t elem_size);

#endif // ERGO_VEC_H

#ifdef ERGO_IMPLEMENTATION
#include <stdio.h> // for NULL check warning

// Implementation of the resize logic
void* ergo_vec_grow(void* v, size_t elem_size) {
    size_t new_cap = v ? VEC_HDR(v)->capacity * 2 : 8; // Start with 8 elements
    size_t total_size = sizeof(ergo_vec_header) + new_cap * elem_size;
    
    ergo_vec_header* new_hdr = NULL;
    
    if (v == NULL) {
        new_hdr = (ergo_vec_header*)malloc(total_size);
        if (new_hdr) {
            new_hdr->size = 0;
        }
    } else {
        ergo_vec_header* old_hdr = VEC_HDR(v);
        new_hdr = (ergo_vec_header*)realloc(old_hdr, total_size);
    }
    
    if (new_hdr == NULL) {
        fprintf(stderr, "ERROR: ergo_vec_grow failed to allocate memory.\n");
        return NULL;
    }
    
    new_hdr->capacity = new_cap;
    // Return pointer to data section (after the header)
    return (void*)((char*)new_hdr + sizeof(ergo_vec_header));
}
#endif
