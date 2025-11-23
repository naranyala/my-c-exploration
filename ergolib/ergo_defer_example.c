#include "ergo_defer.h"
#include <stdio.h>
#include <string.h>

void process_file_data(const char* filename) {
    // --- 1. Automatic File Closing ---
    FILE* f = fopen(filename, "r");
    // If 'f' is assigned, ergo_close_file will be called when exiting this function.
    DEFER(ergo_close_file, f); 
    
    if (!f) {
        printf("ERROR: Could not open file.\n");
        return; // 'f' is NULL, ergo_close_file safely handles it.
    }
    
    // --- 2. Automatic Memory Freeing ---
    char* buffer = malloc(1024);
    // If 'buffer' is assigned, ergo_free_resource will be called upon return.
    DEFER(ergo_free_resource, buffer); 

    if (!buffer) {
        printf("ERROR: Out of memory.\n");
        return; 
    }
    
    // Normal resource usage...
    if (fgets(buffer, 1024, f)) {
        printf("Read: %s", buffer);
    }
    
    printf("Leaving function. Resources will be cleaned up automatically.\n");
}
