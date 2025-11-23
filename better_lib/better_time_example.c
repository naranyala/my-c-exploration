
#include "better_time.h"
#include <stdio.h>

int main() {
    bstopwatch t;
    btimer_start(&t);

    bsleep_ms(500);

    printf("Elapsed: %.2f ms\n", btimer_elapsed_ms(&t));
}
