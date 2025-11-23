/* ergo_sync.h — ergonomic pthread wrappers */
#ifndef ERGO_SYNC_H
#define ERGO_SYNC_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    pthread_mutex_t m;
} ergo_mutex;

typedef struct {
    pthread_cond_t c;
} ergo_cond;

/* All return 0 on success */
int ergo_mutex_init(ergo_mutex *m);
int ergo_mutex_lock(ergo_mutex *m);
int ergo_mutex_unlock(ergo_mutex *m);
int ergo_mutex_destroy(ergo_mutex *m);

int ergo_cond_init(ergo_cond *c);
int ergo_cond_wait(ergo_cond *c, ergo_mutex *m);
int ergo_cond_signal(ergo_cond *c);
int ergo_cond_broadcast(ergo_cond *c);
int ergo_cond_destroy(ergo_cond *c);

#ifdef __cplusplus
}
#endif
#endif /* ERGO_SYNC_H */

#ifdef ERGO_SYNC_IMPLEMENTATION
#include <pthread.h>

int ergo_mutex_init(ergo_mutex *m){ return pthread_mutex_init(&m->m, NULL); }
int ergo_mutex_lock(ergo_mutex *m){ return pthread_mutex_lock(&m->m); }
int ergo_mutex_unlock(ergo_mutex *m){ return pthread_mutex_unlock(&m->m); }
int ergo_mutex_destroy(ergo_mutex *m){ return pthread_mutex_destroy(&m->m); }

int ergo_cond_init(ergo_cond *c){ return pthread_cond_init(&c->c, NULL); }
int ergo_cond_wait(ergo_cond *c, ergo_mutex *m){ return pthread_cond_wait(&c->c, &m->m); }
int ergo_cond_signal(ergo_cond *c){ return pthread_cond_signal(&c->c); }
int ergo_cond_broadcast(ergo_cond *c){ return pthread_cond_broadcast(&c->c); }
int ergo_cond_destroy(ergo_cond *c){ return pthread_cond_destroy(&c->c); }

#endif /* ERGO_SYNC_IMPLEMENTATION */

