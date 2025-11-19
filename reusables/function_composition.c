
#include <stdio.h>

// Function declaration
int add(int a, int b);

int main() {
  int x = 5, y = 10;
  printf("Sum: %d\n", add(x, y));
  return 0;
}

// Function definition
int add(int a, int b) { return a + b; }
