#include "smart_rand.h"
#include <stdio.h>

int main() {
    sr128_state rng = sr_init(123456);

    printf("u32: %u\n", sr_rand_u32(&rng));
    printf("float01: %f\n", sr_rand_float(&rng));
    printf("range 10..20: %d\n", sr_rand_rangei(&rng, 10, 20));
    printf("range 0..1: %f\n", sr_rand_rangef(&rng, 0.f, 1.f));

    const char* names[] = {"Ana", "Budi", "Citra", "Dio"};
    const char* pick = *(const char**)sr_choice(&rng, names, 4, sizeof(char*));
    printf("choice: %s\n", pick);
    
    return 0;
}
