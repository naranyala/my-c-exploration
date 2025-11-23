#define BETTER_TIME_IMPLEMENTATION
#include "better_time5.h"
#include <stdio.h>

int main() {
    uint64_t start = bt_now_us();
    
    printf("Sleeping for 200ms...\n");
    bt_sleep_ms(200);
    
    uint64_t end = bt_now_us();
    printf("Actual elapsed: %lu us\n", end - start);
    
    return 0;
}
