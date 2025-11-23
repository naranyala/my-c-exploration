#ifndef ERGO_THREAD_H
#define ERGO_THREAD_H

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

// Simple thread runner
typedef void *(*ergo_thread_fn)(void *);

static inline pthread_t ergo_thread_start(ergo_thread_fn fn, void *arg) {
    pthread_t tid;
    if (pthread_create(&tid, NULL, fn, arg) != 0) {
        perror("ergo_thread: pthread_create failed");
        exit(1);
    }
    return tid;
}

static inline void ergo_thread_join(pthread_t tid) {
    if (pthread_join(tid, NULL) != 0) {
        perror("ergo_thread: pthread_join failed");
        exit(1);
    }
}

#endif // ERGO_THREAD_H

