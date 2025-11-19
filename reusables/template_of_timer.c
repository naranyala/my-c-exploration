
#include <stdio.h>
#include <time.h>

int main() {
  clock_t start = clock();

  // Some process
  for (long i = 0; i < 100000000; i++)
    ;

  clock_t end = clock();
  double time_taken = (double)(end - start) / CLOCKS_PER_SEC;
  printf("Time taken: %f seconds\n", time_taken);
  return 0;
}
