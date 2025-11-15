#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Function to generate a random number in range [min, max]
int random_in_range(int min, int max) {
    int range = max - min + 1;
    return min + (rand() % range); // Note: Potential for modulo bias
}

int main() {
    // Seed the random number generator with current time
    srand((unsigned int)time(NULL));

    // Generate random number between 1 and 100
    int random_num = random_in_range(1, 100);

    printf("Your random number: %d\n", random_num);

    return 0;
}
