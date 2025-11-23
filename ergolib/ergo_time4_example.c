/* Usage */
#include "ergo_time4.h"
int main() {
    double start = now_monotonic_sec();
    sleep_ms(1234);
    char buf[32];
    printf("Elapsed: %s\n", duration_str(now_monotonic_sec() - start, buf));
}

