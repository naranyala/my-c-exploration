#define ERGO_RAND_IMPLEMENTATION
#include "ergo_rand.h"
#include <stdio.h>

int main() {
    printf("rand=%llu\n", (unsigned long long)ergo_rand_u64());
    printf("range(10..20)=%lld\n", (long long)ergo_rand_range(10,20));
    return 0;
}

