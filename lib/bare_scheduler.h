
/*
 * bare_scheduler.h - Bare minimum multithreaded task scheduler
 * 
 * Usage:
 * #define BARE_SCHED_IMPLEMENTATION
 * #include "bare_scheduler.h"
 * 
 * int main() {
 *     bare_sched_init(4); // 4 threads
 *     bare_sched_submit(task_func, arg);
 *     bare_sched_wait();
 *     bare_sched_shutdown();
 * }
 */

#ifndef BARE_SCHEDULER_H
#define BARE_SCHEDULER_H

#include <pthread.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*bare_task_func_t)(void* arg);

void bare_sched_init(int num_threads);
void bare_sched_submit(bare_task_func_t func, void* arg);
void bare_sched_wait(void);
void bare_sched_shutdown(void);

#ifdef __cplusplus
}
#endif

#endif /* BARE_SCHEDULER_H */

#ifdef BARE_SCHED_IMPLEMENTATION

typedef struct bare_task {
    bare_task_func_t func;
    void* arg;
    struct bare_task* next;
} bare_task_t;

typedef struct {
    bare_task_t* head;
    bare_task_t* tail;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int count;
} bare_queue_t;

typedef struct {
    pthread_t* threads;
    int thread_count;
    int running;
    bare_queue_t queue;
    int pending_tasks;
    pthread_mutex_t pending_mutex;
    pthread_cond_t pending_cond;
} bare_scheduler_t;

static bare_scheduler_t* bare_sched = NULL;

static void bare_queue_push(bare_queue_t* q, bare_task_t* task) {
    pthread_mutex_lock(&q->mutex);
    task->next = NULL;
    
    if (q->tail) {
        q->tail->next = task;
        q->tail = task;
    } else {
        q->head = q->tail = task;
    }
    q->count++;
    
    pthread_cond_signal(&q->cond);
    pthread_mutex_unlock(&q->mutex);
}

static bare_task_t* bare_queue_pop(bare_queue_t* q) {
    pthread_mutex_lock(&q->mutex);
    
    while (q->count == 0 && bare_sched->running) {
        pthread_cond_wait(&q->cond, &q->mutex);
    }
    
    bare_task_t* task = q->head;
    if (task) {
        q->head = task->next;
        if (!q->head) q->tail = NULL;
        q->count--;
    }
    
    pthread_mutex_unlock(&q->mutex);
    return task;
}

static void* bare_worker(void* arg) {
    (void)arg;
    
    while (bare_sched->running) {
        bare_task_t* task = bare_queue_pop(&bare_sched->queue);
        if (!task) continue;
        
        task->func(task->arg);
        free(task);
        
        pthread_mutex_lock(&bare_sched->pending_mutex);
        bare_sched->pending_tasks--;
        if (bare_sched->pending_tasks == 0) {
            pthread_cond_signal(&bare_sched->pending_cond);
        }
        pthread_mutex_unlock(&bare_sched->pending_mutex);
    }
    return NULL;
}

void bare_sched_init(int num_threads) {
    if (bare_sched) return;
    
    bare_sched = malloc(sizeof(bare_scheduler_t));
    bare_sched->thread_count = num_threads > 0 ? num_threads : 4;
    bare_sched->running = 1;
    bare_sched->pending_tasks = 0;
    
    pthread_mutex_init(&bare_sched->queue.mutex, NULL);
    pthread_cond_init(&bare_sched->queue.cond, NULL);
    pthread_mutex_init(&bare_sched->pending_mutex, NULL);
    pthread_cond_init(&bare_sched->pending_cond, NULL);
    
    bare_sched->queue.head = bare_sched->queue.tail = NULL;
    bare_sched->queue.count = 0;
    
    bare_sched->threads = malloc(sizeof(pthread_t) * bare_sched->thread_count);
    for (int i = 0; i < bare_sched->thread_count; i++) {
        pthread_create(&bare_sched->threads[i], NULL, bare_worker, NULL);
    }
}

void bare_sched_submit(bare_task_func_t func, void* arg) {
    if (!bare_sched || !func) return;
    
    bare_task_t* task = malloc(sizeof(bare_task_t));
    task->func = func;
    task->arg = arg;
    task->next = NULL;
    
    pthread_mutex_lock(&bare_sched->pending_mutex);
    bare_sched->pending_tasks++;
    pthread_mutex_unlock(&bare_sched->pending_mutex);
    
    bare_queue_push(&bare_sched->queue, task);
}

void bare_sched_wait(void) {
    if (!bare_sched) return;
    
    pthread_mutex_lock(&bare_sched->pending_mutex);
    while (bare_sched->pending_tasks > 0) {
        pthread_cond_wait(&bare_sched->pending_cond, &bare_sched->pending_mutex);
    }
    pthread_mutex_unlock(&bare_sched->pending_mutex);
}

void bare_sched_shutdown(void) {
    if (!bare_sched) return;
    
    bare_sched->running = 0;
    
    // Wake all threads
    pthread_mutex_lock(&bare_sched->queue.mutex);
    pthread_cond_broadcast(&bare_sched->queue.cond);
    pthread_mutex_unlock(&bare_sched->queue.mutex);
    
    for (int i = 0; i < bare_sched->thread_count; i++) {
        pthread_join(bare_sched->threads[i], NULL);
    }
    
    // Cleanup remaining tasks
    bare_task_t* task = bare_sched->queue.head;
    while (task) {
        bare_task_t* next = task->next;
        free(task);
        task = next;
    }
    
    free(bare_sched->threads);
    pthread_mutex_destroy(&bare_sched->queue.mutex);
    pthread_cond_destroy(&bare_sched->queue.cond);
    pthread_mutex_destroy(&bare_sched->pending_mutex);
    pthread_cond_destroy(&bare_sched->pending_cond);
    free(bare_sched);
    bare_sched = NULL;
}

#endif /* BARE_SCHED_IMPLEMENTATION */
