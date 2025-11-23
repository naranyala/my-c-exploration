
#ifndef BETTER_RANDOM_H
#define BETTER_RANDOM_H

#include <stdlib.h>
#include <time.h>

// Seed once
static inline void random_seed(void) {
    srand((unsigned)time(NULL));
}

// Random int in range [min, max]
static inline int random_range(int min, int max) {
    return min + rand() % (max - min + 1);
}

// Random float in [0,1)
static inline double random_float(void) {
    return rand() / (double)RAND_MAX;
}

#endif // BETTER_RANDOM_H
