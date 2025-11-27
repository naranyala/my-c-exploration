#ifndef SIMPLE_TASK_H
#define SIMPLE_TASK_H

#include <threads.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*task_func_t)(void*);

typedef struct task_s {
    task_func_t func;
    void* arg;
    struct task_s* next;
} task_t;

typedef struct {
    task_t* head;
    task_t* tail;
    mtx_t mtx;
    cnd_t cnd;
    thrd_t thread;
    int stop;
} task_scheduler_t;

// Initialize task scheduler. Returns 0 on success.
int scheduler_init(task_scheduler_t* scheduler);

// Schedule a task to run asynchronously.
int scheduler_schedule(task_scheduler_t* scheduler, task_func_t func, void* arg);

// Shutdown scheduler and wait for thread to finish.
void scheduler_shutdown(task_scheduler_t* scheduler);

#ifdef __cplusplus
}
#endif

#endif // SIMPLE_TASK_H

#ifdef SIMPLE_TASK_IMPLEMENTATION

#include <stdio.h>

static int worker_thread(void* arg) {
    task_scheduler_t* scheduler = (task_scheduler_t*)arg;
    mtx_lock(&scheduler->mtx);
    while (!scheduler->stop) {
        while (!scheduler->head && !scheduler->stop) {
            cnd_wait(&scheduler->cnd, &scheduler->mtx);
        }
        if (scheduler->stop) break;
        task_t* task = scheduler->head;
        if (task) {
            scheduler->head = task->next;
            if (!scheduler->head) scheduler->tail = NULL;
            mtx_unlock(&scheduler->mtx);
            task->func(task->arg);
            free(task);
            mtx_lock(&scheduler->mtx);
        }
    }
    mtx_unlock(&scheduler->mtx);
    return 0;
}

int scheduler_init(task_scheduler_t* scheduler) {
    if (!scheduler) return -1;
    scheduler->head = NULL;
    scheduler->tail = NULL;
    scheduler->stop = 0;
    if (mtx_init(&scheduler->mtx, mtx_plain) != thrd_success) {
        return -1;
    }
    if (cnd_init(&scheduler->cnd) != thrd_success) {
        mtx_destroy(&scheduler->mtx);
        return -1;
    }
    if (thrd_create(&scheduler->thread, worker_thread, scheduler) != thrd_success) {
        cnd_destroy(&scheduler->cnd);
        mtx_destroy(&scheduler->mtx);
        return -1;
    }
    return 0;
}

int scheduler_schedule(task_scheduler_t* scheduler, task_func_t func, void* arg) {
    if (!scheduler || !func) return -1;
    task_t* task = (task_t*)malloc(sizeof(task_t));
    if (!task) return -1;
    task->func = func;
    task->arg = arg;
    task->next = NULL;
    mtx_lock(&scheduler->mtx);
    if (scheduler->tail) {
        scheduler->tail->next = task;
        scheduler->tail = task;
    } else {
        scheduler->head = scheduler->tail = task;
    }
    cnd_signal(&scheduler->cnd);
    mtx_unlock(&scheduler->mtx);
    return 0;
}

void scheduler_shutdown(task_scheduler_t* scheduler) {
    if (!scheduler) return;
    mtx_lock(&scheduler->mtx);
    scheduler->stop = 1;
    cnd_signal(&scheduler->cnd);
    mtx_unlock(&scheduler->mtx);
    thrd_join(scheduler->thread, NULL);
    cnd_destroy(&scheduler->cnd);
    mtx_destroy(&scheduler->mtx);
    // Cleanup remaining tasks if any
    while (scheduler->head) {
        task_t* t = scheduler->head;
        scheduler->head = t->next;
        free(t);
    }
    scheduler->tail = NULL;
}

#endif // SIMPLE_TASK_IMPLEMENTATION

