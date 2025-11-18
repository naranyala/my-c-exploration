
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void compressFile(const char *inputFile, const char *outputFile) {
    FILE *in = fopen(inputFile, "r");
    FILE *out = fopen(outputFile, "w");
    if (in == NULL || out == NULL) {
        perror("Error opening file");
        return;
    }
    char current, prev;
    int count = 1;
    prev = fgetc(in);
    while ((current = fgetc(in)) != EOF) {
        if (current == prev) {
            count++;
        } else {
            fprintf(out, "%d%c", count, prev);
            prev = current;
            count = 1;
        }
    }
    fprintf(out, "%d%c", count, prev);
    fclose(in);
    fclose(out);
}
