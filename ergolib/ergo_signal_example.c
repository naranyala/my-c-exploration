#include "ergo_signal.h"
#include <unistd.h>

void on_sigint(int sig) {
    printf("Caught SIGINT (%d), exiting...\n", sig);
    _exit(0);
}

int main() {
    ergo_signal_handle(SIGINT, on_sigint);
    while (1) pause(); // wait for signals
}

