
#include "better_rwlock.h"
#include <stdio.h>
#include <pthread.h>

brwlock lock;
int shared_value = 0;

void *reader(void *_) {
    for (int i = 0; i < 5; ++i) {
        brw_rlock(&lock);
        printf("read: %d\n", shared_value);
        brw_runlock(&lock);
    }
    return NULL;
}

void *writer(void *_) {
    for (int i = 0; i < 5; ++i) {
        brw_wlock(&lock);
        shared_value++;
        printf("write: %d\n", shared_value);
        brw_wunlock(&lock);
    }
    return NULL;
}

int main() {
    brw_init(&lock);

    pthread_t r, w;
    pthread_create(&r, NULL, reader, NULL);
    pthread_create(&w, NULL, writer, NULL);

    pthread_join(r, NULL);
    pthread_join(w, NULL);

    return 0;
}
