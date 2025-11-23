#define ERGO_TIME_IMPLEMENTATION
#include "ergo_time5.h"
#include <stdio.h>

int main(void){
    int64_t t0 = ergo_now_ms();
    ergo_sleep_ms(150);
    int64_t t1 = ergo_now_ms();
    printf("slept ~%lld ms\n", (long long)(t1 - t0));
    return 0;
}

