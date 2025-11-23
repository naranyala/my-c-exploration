#include "ergo_timer.h"

int main() {
    ergo_timer t = ergo_timer_start();
    for (volatile int i = 0; i < 1000000; i++); // busy loop
    printf("Elapsed: %.2f ms\n", ergo_timer_elapsed_ms(t));
}

