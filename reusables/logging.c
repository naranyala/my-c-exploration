
#include <stdio.h>

#define LOG_INFO(msg) printf("[INFO] %s\n", msg)
#define LOG_ERROR(msg) fprintf(stderr, "[ERROR] %s\n", msg)

int main() {
  LOG_INFO("Program started");
  LOG_ERROR("Something went wrong!");
  return 0;
}
