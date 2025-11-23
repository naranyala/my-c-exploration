
#ifndef BETTER_PTHREAD_H
#define BETTER_PTHREAD_H

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

typedef void* (*thread_fn)(void*);

// Start thread and check errors
static inline pthread_t thread_start(thread_fn fn, void *arg) {
    pthread_t tid;
    if (pthread_create(&tid, NULL, fn, arg) != 0) {
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }
    return tid;
}

// Join thread
static inline void thread_join(pthread_t tid) {
    pthread_join(tid, NULL);
}

#endif // BETTER_PTHREAD_H
