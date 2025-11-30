
#include <stdio.h>
#include <stdlib.h>

int *createArray(int size) {
    int *arr = (int *)malloc(size * sizeof(int));
    if (arr == NULL) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }
    return arr;
}

void freeArray(int *arr) {
    free(arr);
}
