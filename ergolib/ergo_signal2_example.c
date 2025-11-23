/* Usage */
#include "ergo_signal2.h"
#include <stdio.h>

void on_int(int sig) { printf("\nCaught SIGINT, exiting cleanly\n"); exit(0); }

int main() {
    signal_handle(SIGINT, on_int);
    printf("Press Ctrl+C...\n");
    for (;;) pause();
}

