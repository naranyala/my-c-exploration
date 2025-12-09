// status-clock.c
#include <stdio.h>
#include <time.h>
#include <unistd.h>

int main() {
  char buf[256];
  while (1) {
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    strftime(buf, sizeof(buf), "🕒 %H:%M:%S | Built with C ❤️", tm);
    puts(buf);      // adds \n
    fflush(stdout); // critical!
    usleep(500000); // 500ms
  }
  return 0;
}
