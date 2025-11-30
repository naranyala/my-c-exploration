
#include <stdio.h>
#include <stdlib.h>

int main() {
  int n = 5;
  int *arr = (int *)malloc(n * sizeof(int));
  if (arr == NULL) {
    perror("Memory allocation failed");
    return 1;
  }

  for (int i = 0; i < n; i++) {
    arr[i] = i * 2;
    printf("%d ", arr[i]);
  }
  printf("\n");

  free(arr); // Release memory
  return 0;
}
