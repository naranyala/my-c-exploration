#include "ergo_defer3.h"
#include <stdio.h>

int main() {
    FILE *f = fopen("test.txt", "w");
    defer( (void)0, fclose(f) ) {  // end executes after the block ends
        fprintf(f, "Hello!\n");
    }
}

