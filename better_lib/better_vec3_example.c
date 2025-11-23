
#include "better_vec.h"
#include <stdio.h>

int main() {
    bvec v;
    bvec_init(&v);

    for (int i = 0; i < 10; ++i)
        bvec_push(&v, &i, sizeof(int));

    int *arr = (int*)v.data;
    for (size_t i = 0; i < v.len; ++i)
        printf("%d ", arr[i]);

    bvec_free(&v);
}
