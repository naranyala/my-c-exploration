
#include "better_signal3.h"

void on_sigint(int sig) {
    printf("Caught SIGINT (%d)\n", sig);
}

int main() {
    catch_signal(SIGINT, on_sigint);
    while (1) pause();
}
