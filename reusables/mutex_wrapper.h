
// mutex.h
#ifndef MUTEX_H
#define MUTEX_H

#include <pthread.h>

typedef pthread_mutex_t Mutex;

static inline void mutex_init(Mutex *m) { pthread_mutex_init(m, NULL); }
static inline void mutex_lock(Mutex *m) { pthread_mutex_lock(m); }
static inline void mutex_unlock(Mutex *m) { pthread_mutex_unlock(m); }
static inline void mutex_destroy(Mutex *m) { pthread_mutex_destroy(m); }

#endif
