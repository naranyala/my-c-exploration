
#include <stdio.h>
#include <unistd.h> // for sleep

int main() {
  for (int i = 0; i < 5; i++) {
    printf("Tick %d\n", i + 1);
    sleep(1); // wait 1 second
  }
  return 0;
}
