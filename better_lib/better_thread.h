// better_thread.h
#ifndef BETTER_THREAD_H
#define BETTER_THREAD_H

#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

// Auto-join thread on scope exit (GCC/Clang only)
#if defined(__GNUC__) || defined(__clang__)
#define BTHREAD_JOIN(t) __attribute__((cleanup(b_pthread_join))) pthread_t (t)
void b_pthread_join(pthread_t* t);
#endif

// Launch thread with function and arg
int b_thread_create_detached(void* (*func)(void*), void* arg);

#ifdef __cplusplus
}
#endif

#ifdef BETTER_THREAD_IMPL

#if defined(__GNUC__) || defined(__clang__)
void b_pthread_join(pthread_t* t) {
    if (t && *t) pthread_join(*t, NULL);
}
#endif

int b_thread_create_detached(void* (*func)(void*), void* arg) {
    pthread_t t;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    int ret = pthread_create(&t, &attr, func, arg);
    pthread_attr_destroy(&attr);
    return ret; // 0 on success
}

#endif // BETTER_THREAD_IMPL

#endif // BETTER_THREAD_H
