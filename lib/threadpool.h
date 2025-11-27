/*
 * threadpool.h - A simple thread pool implementation for C
 * 
 * Usage:
 *   #define THREADPOOL_IMPLEMENTATION
 *   #include "threadpool.h"
 * 
 * Features:
 *   - Fixed number of worker threads
 *   - Thread-safe task queue
 *   - Wait for completion
 *   - Graceful shutdown
 * 
 * License: MIT
 */

#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <stddef.h>
#include <stdint.h>

#ifdef _WIN32
    #include <windows.h>
    #include <process.h>
#else
    #include <pthread.h>
    #include <unistd.h>
    #include <semaphore.h>
#endif

typedef struct thread_pool_t thread_pool_t;
typedef void (*thread_task_func_t)(void* arg);

#ifdef __cplusplus
extern "C" {
#endif

/* Creation and management */
thread_pool_t* thread_pool_create(size_t num_threads);
void thread_pool_destroy(thread_pool_t* pool);

/* Task submission */
int thread_pool_submit(thread_pool_t* pool, thread_task_func_t func, void* arg);
int thread_pool_try_submit(thread_pool_t* pool, thread_task_func_t func, void* arg);

/* Control and monitoring */
void thread_pool_wait(thread_pool_t* pool);
void thread_pool_shutdown(thread_pool_t* pool);
int thread_pool_is_shutdown(thread_pool_t* pool);
size_t thread_pool_pending_tasks(thread_pool_t* pool);
size_t thread_pool_num_threads(thread_pool_t* pool);

/* Utility functions */
int thread_pool_get_hardware_threads(void);

#ifdef __cplusplus
}
#endif

#endif /* THREADPOOL_H */

#ifdef THREADPOOL_IMPLEMENTATION

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct thread_task_t {
    thread_task_func_t func;
    void* arg;
    struct thread_task_t* next;
} thread_task_t;

typedef struct thread_task_queue_t {
    thread_task_t* head;
    thread_task_t* tail;
    size_t count;
} thread_task_queue_t;

struct thread_pool_t {
#ifdef _WIN32
    HANDLE* threads;
    CRITICAL_SECTION lock;
    CONDITION_VARIABLE task_available;
    CONDITION_VARIABLE task_completed;
#else
    pthread_t* threads;
    pthread_mutex_t lock;
    pthread_cond_t task_available;
    pthread_cond_t task_completed;
#endif
    
    thread_task_queue_t queue;
    size_t num_threads;
    size_t working_count;
    int shutdown;
};

/* Thread-safe queue operations */
static void task_queue_init(thread_task_queue_t* queue) {
    memset(queue, 0, sizeof(thread_task_queue_t));
}

static void task_queue_push(thread_task_queue_t* queue, thread_task_t* task) {
    task->next = NULL;
    
    if (queue->tail) {
        queue->tail->next = task;
    } else {
        queue->head = task;
    }
    queue->tail = task;
    queue->count++;
}

static thread_task_t* task_queue_pop(thread_task_queue_t* queue) {
    thread_task_t* task = queue->head;
    if (task) {
        queue->head = task->next;
        if (!queue->head) {
            queue->tail = NULL;
        }
        queue->count--;
    }
    return task;
}

static void task_queue_clear(thread_task_queue_t* queue) {
    thread_task_t* task = queue->head;
    while (task) {
        thread_task_t* next = task->next;
        free(task);
        task = next;
    }
    queue->head = queue->tail = NULL;
    queue->count = 0;
}

#ifdef _WIN32
static unsigned __stdcall worker_thread(void* arg) {
#else
static void* worker_thread(void* arg) {
#endif
    thread_pool_t* pool = (thread_pool_t*)arg;
    
    while (1) {
#ifdef _WIN32
        EnterCriticalSection(&pool->lock);
#else
        pthread_mutex_lock(&pool->lock);
#endif
        
        /* Wait for tasks or shutdown */
        while (pool->queue.head == NULL && !pool->shutdown) {
#ifdef _WIN32
            SleepConditionVariableCS(&pool->task_available, &pool->lock, INFINITE);
#else
            pthread_cond_wait(&pool->task_available, &pool->lock);
#endif
        }
        
        /* Check for shutdown */
        if (pool->shutdown && pool->queue.head == NULL) {
#ifdef _WIN32
            LeaveCriticalSection(&pool->lock);
#else
            pthread_mutex_unlock(&pool->lock);
#endif
            break;
        }
        
        /* Get a task */
        thread_task_t* task = task_queue_pop(&pool->queue);
        pool->working_count++;
        
#ifdef _WIN32
        LeaveCriticalSection(&pool->lock);
#else
        pthread_mutex_unlock(&pool->lock);
#endif
        
        /* Execute the task */
        if (task) {
            task->func(task->arg);
            free(task);
        }
        
#ifdef _WIN32
        EnterCriticalSection(&pool->lock);
#else
        pthread_mutex_lock(&pool->lock);
#endif
        pool->working_count--;
        
        /* Notify waiters if no more tasks are being processed */
        if (pool->working_count == 0 && pool->queue.head == NULL) {
#ifdef _WIN32
            WakeConditionVariable(&pool->task_completed);
#else
            pthread_cond_broadcast(&pool->task_completed);
#endif
        }
#ifdef _WIN32
        LeaveCriticalSection(&pool->lock);
#else
        pthread_mutex_unlock(&pool->lock);
#endif
    }
    
#ifdef _WIN32
    return 0;
#else
    return NULL;
#endif
}

thread_pool_t* thread_pool_create(size_t num_threads) {
    if (num_threads == 0) {
        num_threads = 1;
    }
    
    thread_pool_t* pool = (thread_pool_t*)calloc(1, sizeof(thread_pool_t));
    if (!pool) return NULL;
    
    pool->num_threads = num_threads;
    task_queue_init(&pool->queue);
    
#ifdef _WIN32
    InitializeCriticalSection(&pool->lock);
    InitializeConditionVariable(&pool->task_available);
    InitializeConditionVariable(&pool->task_completed);
    
    pool->threads = (HANDLE*)malloc(num_threads * sizeof(HANDLE));
    if (!pool->threads) {
        free(pool);
        return NULL;
    }
    
    for (size_t i = 0; i < num_threads; i++) {
        pool->threads[i] = (HANDLE)_beginthreadex(NULL, 0, worker_thread, pool, 0, NULL);
        if (!pool->threads[i]) {
            /* Clean up already created threads */
            for (size_t j = 0; j < i; j++) {
                CloseHandle(pool->threads[j]);
            }
            free(pool->threads);
            free(pool);
            return NULL;
        }
    }
#else
    if (pthread_mutex_init(&pool->lock, NULL) != 0) {
        free(pool);
        return NULL;
    }
    
    if (pthread_cond_init(&pool->task_available, NULL) != 0) {
        pthread_mutex_destroy(&pool->lock);
        free(pool);
        return NULL;
    }
    
    if (pthread_cond_init(&pool->task_completed, NULL) != 0) {
        pthread_mutex_destroy(&pool->lock);
        pthread_cond_destroy(&pool->task_available);
        free(pool);
        return NULL;
    }
    
    pool->threads = (pthread_t*)malloc(num_threads * sizeof(pthread_t));
    if (!pool->threads) {
        pthread_mutex_destroy(&pool->lock);
        pthread_cond_destroy(&pool->task_available);
        pthread_cond_destroy(&pool->task_completed);
        free(pool);
        return NULL;
    }
    
    for (size_t i = 0; i < num_threads; i++) {
        if (pthread_create(&pool->threads[i], NULL, worker_thread, pool) != 0) {
            /* Clean up already created threads */
            pool->shutdown = 1;
#ifdef _WIN32
            WakeAllConditionVariable(&pool->task_available);
#else
            pthread_cond_broadcast(&pool->task_available);
#endif
            for (size_t j = 0; j < i; j++) {
                pthread_join(pool->threads[j], NULL);
            }
            free(pool->threads);
            pthread_mutex_destroy(&pool->lock);
            pthread_cond_destroy(&pool->task_available);
            pthread_cond_destroy(&pool->task_completed);
            free(pool);
            return NULL;
        }
    }
#endif
    
    return pool;
}

void thread_pool_destroy(thread_pool_t* pool) {
    if (!pool) return;
    
    thread_pool_shutdown(pool);
    thread_pool_wait(pool);
    
#ifdef _WIN32
    /* Wait for all threads to finish */
    WaitForMultipleObjects((DWORD)pool->num_threads, pool->threads, TRUE, INFINITE);
    
    /* Clean up thread handles */
    for (size_t i = 0; i < pool->num_threads; i++) {
        CloseHandle(pool->threads[i]);
    }
    
    DeleteCriticalSection(&pool->lock);
    /* Condition variables don't need cleanup on Windows */
#else
    /* Wait for all threads to finish */
    for (size_t i = 0; i < pool->num_threads; i++) {
        pthread_join(pool->threads[i], NULL);
    }
    
    pthread_mutex_destroy(&pool->lock);
    pthread_cond_destroy(&pool->task_available);
    pthread_cond_destroy(&pool->task_completed);
#endif
    
    /* Clean up any remaining tasks */
    task_queue_clear(&pool->queue);
    
    free(pool->threads);
    free(pool);
}

int thread_pool_submit(thread_pool_t* pool, thread_task_func_t func, void* arg) {
    if (!pool || !func || pool->shutdown) return 0;
    
    thread_task_t* task = (thread_task_t*)malloc(sizeof(thread_task_t));
    if (!task) return 0;
    
    task->func = func;
    task->arg = arg;
    task->next = NULL;
    
#ifdef _WIN32
    EnterCriticalSection(&pool->lock);
#else
    pthread_mutex_lock(&pool->lock);
#endif
    
    task_queue_push(&pool->queue, task);
    
#ifdef _WIN32
    WakeConditionVariable(&pool->task_available);
    LeaveCriticalSection(&pool->lock);
#else
    pthread_cond_signal(&pool->task_available);
    pthread_mutex_unlock(&pool->lock);
#endif
    
    return 1;
}

int thread_pool_try_submit(thread_pool_t* pool, thread_task_func_t func, void* arg) {
    if (!pool || !func || pool->shutdown) return 0;
    
    thread_task_t* task = (thread_task_t*)malloc(sizeof(thread_task_t));
    if (!task) return 0;
    
    task->func = func;
    task->arg = arg;
    task->next = NULL;
    
#ifdef _WIN32
    EnterCriticalSection(&pool->lock);
#else
    pthread_mutex_lock(&pool->lock);
#endif
    
    int success = 1;
    if (pool->shutdown) {
        success = 0;
        free(task);
    } else {
        task_queue_push(&pool->queue, task);
#ifdef _WIN32
        WakeConditionVariable(&pool->task_available);
#else
        pthread_cond_signal(&pool->task_available);
#endif
    }
    
#ifdef _WIN32
    LeaveCriticalSection(&pool->lock);
#else
    pthread_mutex_unlock(&pool->lock);
#endif
    
    return success;
}

void thread_pool_wait(thread_pool_t* pool) {
    if (!pool) return;
    
#ifdef _WIN32
    EnterCriticalSection(&pool->lock);
#else
    pthread_mutex_lock(&pool->lock);
#endif
    
    while ((pool->queue.head != NULL || pool->working_count > 0) && !pool->shutdown) {
#ifdef _WIN32
        SleepConditionVariableCS(&pool->task_completed, &pool->lock, INFINITE);
#else
        pthread_cond_wait(&pool->task_completed, &pool->lock);
#endif
    }
    
#ifdef _WIN32
    LeaveCriticalSection(&pool->lock);
#else
    pthread_mutex_unlock(&pool->lock);
#endif
}

void thread_pool_shutdown(thread_pool_t* pool) {
    if (!pool) return;
    
#ifdef _WIN32
    EnterCriticalSection(&pool->lock);
#else
    pthread_mutex_lock(&pool->lock);
#endif
    
    pool->shutdown = 1;
    
#ifdef _WIN32
    WakeAllConditionVariable(&pool->task_available);
    LeaveCriticalSection(&pool->lock);
#else
    pthread_cond_broadcast(&pool->task_available);
    pthread_mutex_unlock(&pool->lock);
#endif
}

int thread_pool_is_shutdown(thread_pool_t* pool) {
    if (!pool) return 1;
    
#ifdef _WIN32
    EnterCriticalSection(&pool->lock);
    int shutdown = pool->shutdown;
    LeaveCriticalSection(&pool->lock);
#else
    pthread_mutex_lock(&pool->lock);
    int shutdown = pool->shutdown;
    pthread_mutex_unlock(&pool->lock);
#endif
    
    return shutdown;
}

size_t thread_pool_pending_tasks(thread_pool_t* pool) {
    if (!pool) return 0;
    
#ifdef _WIN32
    EnterCriticalSection(&pool->lock);
    size_t count = pool->queue.count;
    LeaveCriticalSection(&pool->lock);
#else
    pthread_mutex_lock(&pool->lock);
    size_t count = pool->queue.count;
    pthread_mutex_unlock(&pool->lock);
#endif
    
    return count;
}

size_t thread_pool_num_threads(thread_pool_t* pool) {
    return pool ? pool->num_threads : 0;
}

int thread_pool_get_hardware_threads(void) {
#ifdef _WIN32
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    return (int)sysinfo.dwNumberOfProcessors;
#else
    long cores = sysconf(_SC_NPROCESSORS_ONLN);
    return cores > 0 ? (int)cores : 1;
#endif
}

#endif /* THREADPOOL_IMPLEMENTATION */
