#include "ergo_thread.h"

void *worker(void *arg) {
    printf("Hello from thread! arg=%s\n", (char*)arg);
    return NULL;
}

int main() {
    pthread_t t = ergo_thread_start(worker, "Fudzer");
    ergo_thread_join(t);
}

