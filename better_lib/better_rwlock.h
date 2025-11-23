
/* better_rwlock.h */

#ifndef BETTER_RWLOCK_H
#define BETTER_RWLOCK_H

#ifdef _WIN32

#include <windows.h>

typedef SRWLOCK brwlock;

static inline void brw_init(brwlock *l) { InitializeSRWLock(l); }
static inline void brw_rlock(brwlock *l) { AcquireSRWLockShared(l); }
static inline void brw_runlock(brwlock *l) { ReleaseSRWLockShared(l); }
static inline void brw_wlock(brwlock *l) { AcquireSRWLockExclusive(l); }
static inline void brw_wunlock(brwlock *l) { ReleaseSRWLockExclusive(l); }

#else
#ifdef __unix__

#include <pthread.h>

typedef pthread_rwlock_t brwlock;

static inline void brw_init(brwlock *l) { pthread_rwlock_init(l, NULL); }
static inline void brw_rlock(brwlock *l) { pthread_rwlock_rdlock(l); }
static inline void brw_runlock(brwlock *l) { pthread_rwlock_unlock(l); }
static inline void brw_wlock(brwlock *l) { pthread_rwlock_wrlock(l); }
static inline void brw_wunlock(brwlock *l) { pthread_rwlock_unlock(l); }

#else
/* Generic fallback implementation */
#include <pthread.h>

typedef struct {
    pthread_mutex_t m;
    pthread_cond_t  c;
    int readers;
    int writer;
} brwlock;

static inline void brw_init(brwlock *l) {
    pthread_mutex_init(&l->m, NULL);
    pthread_cond_init(&l->c, NULL);
    l->readers = 0;
    l->writer = 0;
}

static inline void brw_rlock(brwlock *l) {
    pthread_mutex_lock(&l->m);
    while (l->writer)
        pthread_cond_wait(&l->c, &l->m);
    l->readers++;
    pthread_mutex_unlock(&l->m);
}

static inline void brw_runlock(brwlock *l) {
    pthread_mutex_lock(&l->m);
    l->readers--;
    if (l->readers == 0)
        pthread_cond_signal(&l->c);
    pthread_mutex_unlock(&l->m);
}

static inline void brw_wlock(brwlock *l) {
    pthread_mutex_lock(&l->m);
    while (l->writer || l->readers)
        pthread_cond_wait(&l->c, &l->m);
    l->writer = 1;
    pthread_mutex_unlock(&l->m);
}

static inline void brw_wunlock(brwlock *l) {
    pthread_mutex_lock(&l->m);
    l->writer = 0;
    pthread_cond_broadcast(&l->c);
    pthread_mutex_unlock(&l->m);
}

#endif
#endif

#endif
