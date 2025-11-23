#define BETTER_CHAN_IMPLEMENTATION
#include "better_chan.h"
#include <stdio.h>
#include <unistd.h>

void* worker(void* arg) {
    b_chan* ch = (b_chan*)arg;
    while(1) {
        char* msg = (char*)bc_recv(ch); // Blocks waiting for work
        printf("Worker received: %s\n", msg);
        if (msg == NULL) break; // Poison pill to stop
    }
    return NULL;
}

int main() {
    b_chan* ch = bc_create();
    pthread_t t;
    pthread_create(&t, NULL, worker, ch);

    bc_send(ch, "Job 1");
    sleep(1);
    bc_send(ch, "Job 2");
    
    pthread_join(t, NULL);
    bc_destroy(ch);
    return 0;
}
