
#include "better_signal.h"
#include <stdio.h>

void on_int(int s) {
    printf("Caught SIGINT!\n");
}

int main() {
    bsig_register(SIGINT, on_int);
    printf("Press Ctrl+C...\n");

    for (;;) {}
}
