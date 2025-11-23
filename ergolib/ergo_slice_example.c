#include "ergo_slice.h"
#include <stdio.h>

int main() {
    int nums[] = {1, 2, 3, 4};
    ergo_slice s = ergo_slice_of(nums);

    for (size_t i = 0; i < s.len; i++) {
        int *v = ergo_slice_at(s, i);
        printf("%d\n", *v);
    }
}

