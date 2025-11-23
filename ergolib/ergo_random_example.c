

/* Usage */
#include "ergo_random.h"
#include <stdio.h>

int main() {
    uint8_t key[32];
    rnd_bytes(key, sizeof(key));
    printf("Random session ID: %016llx\n", (unsigned long long)rnd_u64());
}

