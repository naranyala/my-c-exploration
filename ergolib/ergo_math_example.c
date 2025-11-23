#include "ergo_math.h"
#include <stdio.h>

int main() {
    printf("Clamp: %.2f\n", ergo_clamp(15, 0, 10));
    printf("Lerp: %.2f\n", ergo_lerp(0, 100, 0.25));
    printf("Map: %.2f\n", ergo_map(5, 0, 10, 0, 1));
}

