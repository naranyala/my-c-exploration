/* Example usage */
#include "ergo_defer2.h"
#include <stdio.h>
#include <stdlib.h>

static void close_file(void *f) { fclose((FILE*)f); }
static void free_ptr(void *p) { free(*(void**)p); }

int main() {
    FILE *f = fopen("data.txt", "r");
    if (!f) return 1;
    DEFER_ARG(close_file, f);

    void *data = malloc(1024);
    DEFER_ARG(free_ptr, &data);

    // ... use f and data safely ...
    printf("Everything auto-cleaned up!\n");
    return 0;  // fclose and free called automatically
}

