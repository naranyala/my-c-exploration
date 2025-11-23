
#include "better_time4.h"

int main() {
    char buf[64];
    time_to_iso8601(buf, sizeof(buf));
    printf("Now: %s\n", buf);

    stopwatch_t sw;
    stopwatch_start(&sw);
    for (volatile int i = 0; i < 100000000; i++); // busy loop
    printf("Elapsed: %.3f sec\n", stopwatch_elapsed(&sw));
}
