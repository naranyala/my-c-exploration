#define BETTER_EXEC_IMPLEMENTATION
#include "better_exec.h"
#include <stdio.h>

int main() {
    // Run ls -la and capture output
    char* output = be_exec_read("ls -la");
    
    if (output) {
        printf("Directory Listing:\n%s", output);
        free(output);
    }
    return 0;
}
