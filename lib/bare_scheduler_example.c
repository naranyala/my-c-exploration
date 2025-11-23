// bare_example.c
#define BARE_SCHED_IMPLEMENTATION
#include "bare_scheduler.h"
#include <stdio.h>
#include <unistd.h>

void bare_task(void* arg) {
    int id = *(int*)arg;
    printf("Task %d running\n", id);
    usleep(100000); // 100ms work
}

int main() {
    bare_sched_init(4); // 4 threads
    
    int ids[10];
    for (int i = 0; i < 10; i++) {
        ids[i] = i;
        bare_sched_submit(bare_task, &ids[i]);
    }
    
    bare_sched_wait();
    printf("All tasks done!\n");
    
    bare_sched_shutdown();
    return 0;
}
