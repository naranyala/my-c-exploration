#include "ergo_lock.h"
#include <stdio.h>

ergo_lock lock;

void *worker(void *arg) {
    ergo_lock_acquire(&lock);
    printf("Thread %s working...\n", (char*)arg);
    ergo_lock_release(&lock);
    return NULL;
}

