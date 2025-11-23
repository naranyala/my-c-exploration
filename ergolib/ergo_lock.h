#ifndef ERGO_LOCK_H
#define ERGO_LOCK_H

#include <pthread.h>

typedef struct {
    pthread_mutex_t m;
} ergo_lock;

static inline void ergo_lock_init(ergo_lock *l) {
    pthread_mutex_init(&l->m, NULL);
}

static inline void ergo_lock_acquire(ergo_lock *l) {
    pthread_mutex_lock(&l->m);
}

static inline void ergo_lock_release(ergo_lock *l) {
    pthread_mutex_unlock(&l->m);
}

static inline void ergo_lock_destroy(ergo_lock *l) {
    pthread_mutex_destroy(&l->m);
}

#endif // ERGO_LOCK_H

