
#include "better_pthread.h"

void* worker(void *arg) {
    printf("Thread says hi!\n");
    return NULL;
}

int main() {
    pthread_t tid = thread_start(worker, NULL);
    thread_join(tid);
}
