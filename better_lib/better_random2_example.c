
#include "better_random2.h"
#include <stdio.h>

int main() {
    random_seed();
    printf("Dice roll: %d\n", random_range(1, 6));
    printf("Random float: %f\n", random_float());
}
