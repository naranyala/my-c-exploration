#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_JOBS 5
#define NUM_WORKERS 3

// Shared data structure
typedef struct {
    int job_id;
    int result;
} job_t;

// Worker thread arguments
typedef struct {
    int worker_id;
    job_t *jobs;
    int *job_index;
    int *results_ready;
    pthread_mutex_t *mutex;
    pthread_cond_t *cond;
} worker_args_t;

// Worker thread function
void* worker(void* arg) {
    worker_args_t* args = (worker_args_t*)arg;
    int worker_id = args->worker_id;
    
    while (1) {
        pthread_mutex_lock(args->mutex);
        
        // Check if there are more jobs
        if (*args->job_index >= NUM_JOBS) {
            pthread_mutex_unlock(args->mutex);
            break;
        }
        
        // Get the next job
        int job_num = (*args->job_index) + 1;
        (*args->job_index)++;
        pthread_mutex_unlock(args->mutex);
        
        printf("worker %d started job %d\n", worker_id, job_num);
        sleep(1);  // Simulate work
        printf("worker %d finished job %d\n", worker_id, job_num);
        
        // Store result
        pthread_mutex_lock(args->mutex);
        args->jobs[job_num-1].result = job_num * 2;
        (*args->results_ready)++;
        pthread_cond_signal(args->cond);
        pthread_mutex_unlock(args->mutex);
    }
    
    return NULL;
}

int main() {
    pthread_t workers[NUM_WORKERS];
    worker_args_t worker_args[NUM_WORKERS];
    job_t jobs[NUM_JOBS];
    
    // Initialize synchronization primitives
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
    
    int job_index = 0;
    int results_ready = 0;
    
    // Initialize jobs
    for (int i = 0; i < NUM_JOBS; i++) {
        jobs[i].job_id = i + 1;
        jobs[i].result = 0;
    }
    
    // Create worker threads
    for (int w = 0; w < NUM_WORKERS; w++) {
        worker_args[w].worker_id = w + 1;
        worker_args[w].jobs = jobs;
        worker_args[w].job_index = &job_index;
        worker_args[w].results_ready = &results_ready;
        worker_args[w].mutex = &mutex;
        worker_args[w].cond = &cond;
        
        if (pthread_create(&workers[w], NULL, worker, &worker_args[w]) != 0) {
            perror("Failed to create thread");
            exit(1);
        }
    }
    
    // Wait for all results
    pthread_mutex_lock(&mutex);
    while (results_ready < NUM_JOBS) {
        pthread_cond_wait(&cond, &mutex);
    }
    pthread_mutex_unlock(&mutex);
    
    // Wait for all threads to finish
    for (int w = 0; w < NUM_WORKERS; w++) {
        pthread_join(workers[w], NULL);
    }
    
    // Print results (optional)
    printf("\nAll jobs completed. Results:\n");
    for (int i = 0; i < NUM_JOBS; i++) {
        printf("Job %d result: %d\n", jobs[i].job_id, jobs[i].result);
    }
    
    // Cleanup
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
    
    return 0;
}
