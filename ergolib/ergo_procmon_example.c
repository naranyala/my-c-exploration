#include "ergo_procmon.h"
#include <stdio.h>

int main() {
    printf("Mem: %ld KB\n", ergo_proc_mem_kb());
    printf("CPU load: %f\n", ergo_proc_cpu_load());
}

