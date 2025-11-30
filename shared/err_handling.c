
#include <stdio.h>
#include <errno.h>
#include <string.h>

void openFile(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening file: %s\n", strerror(errno));
        return;
    }
    printf("File opened successfully!\n");
    fclose(file);
}
