
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

void *threadFunction(void *arg) {
    int threadId = *(int *)arg;
    printf("Thread %d is running\n", threadId);
    sleep(1);
    printf("Thread %d is exiting\n", threadId);
    return NULL;
}

int main() {
    pthread_t threads[3];
    int threadIds[3] = {1, 2, 3};
    for (int i = 0; i < 3; i++) {
        pthread_create(&threads[i], NULL, threadFunction, &threadIds[i]);
    }
    for (int i = 0; i < 3; i++) {
        pthread_join(threads[i], NULL);
    }
    return 0;
}
