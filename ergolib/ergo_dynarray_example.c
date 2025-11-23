#include "ergo_dynarray.h"
#include <stdio.h>

int main() {
    ergo_dynarray arr;
    ergo_dynarray_init(&arr, sizeof(int));
    for (int i = 0; i < 5; i++) ergo_dynarray_push(&arr, &i);
    for (size_t i = 0; i < arr.len; i++) {
        printf("%d\n", *(int*)ergo_dynarray_get(&arr, i));
    }
    ergo_dynarray_free(&arr);
}

