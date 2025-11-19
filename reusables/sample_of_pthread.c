
#include <pthread.h>
#include <stdio.h>

void *threadFunc(void *arg) {
  printf("Hello from thread %d\n", *(int *)arg);
  return NULL;
}

int main() {
  pthread_t t1, t2;
  int id1 = 1, id2 = 2;
  pthread_create(&t1, NULL, threadFunc, &id1);
  pthread_create(&t2, NULL, threadFunc, &id2);
  pthread_join(t1, NULL);
  pthread_join(t2, NULL);
  return 0;
}
