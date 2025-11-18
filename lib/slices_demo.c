#include <stdio.h>
#include <stdlib.h>
#include "slices.h"

int int_cmp(const void *a, const void *b) {
    return (*(const int *)a) - (*(const int *)b);
}

int main() {
    // Test array
    int arr[] = {1, 2, 3, 4, 5};
    slice(int) s = slice_from_array(arr, 5);

    // Test slicing
    slice(int) sub = slice_slice(s, 1, 3);
    printf("Subslice: ");
    slice_foreach(sub, it) {
        printf("%d ", *it);
    }
    printf("\n"); // Expected: 2 3

    // Test empty slice
    slice(int) empty = slice_empty(int);
    printf("Empty first: %d\n", slice_first(empty, -1)); // Expected: -1

    // Test comparison
    slice(int) s2 = slice_from_array(arr, 5);
    printf("Equal: %d\n", slice_equal(s, s2, int_cmp)); // Expected: 1

    // Test string slice
    str_slice str = str_slice_from_literal("hello");
    char buf[10];
    str_slice_to_cstr(str, buf, sizeof(buf));
    printf("String: %s\n", buf); // Expected: hello

    // Test find
    int value = 3;
    size_t idx = slice_find(s, value, NULL);
    printf("Found %d at index: %zu\n", value, idx); // Expected: 2

    return 0;
}
