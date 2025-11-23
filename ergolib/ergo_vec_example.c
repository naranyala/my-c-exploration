#include "ergo_vec.h"
#include <stdio.h>

int main() {
    int* my_int_vec = NULL; // Must start as NULL for initialization
    
    // The macro handles allocation, type casting, and header setup.
    ergo_vec_push(my_int_vec, 10);
    ergo_vec_push(my_int_vec, 20);
    ergo_vec_push(my_int_vec, 30);
    
    printf("Vector Length: %zu\n", ergo_vec_len(my_int_vec));
    
    // Standard C loop for iteration
    for (size_t i = 0; i < ergo_vec_len(my_int_vec); i++) {
        printf("Element %zu: %d\n", i, my_int_vec[i]);
    }
    
    // Clean up
    ergo_vec_free(my_int_vec);
    
    return 0;
}
