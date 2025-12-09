
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // for sleep()

int main() {
  while (1) {
    FILE *cpu = fopen("/proc/stat", "r");
    FILE *mem = fopen("/proc/meminfo", "r");
    char line[256];
    long user, nice, system, idle, total_mem, free_mem;

    // Read CPU usage
    fgets(line, sizeof(line), cpu);
    sscanf(line, "cpu %ld %ld %ld %ld", &user, &nice, &system, &idle);
    long total_cpu = user + nice + system + idle;
    long used_cpu = user + nice + system;
    float cpu_usage = (float)used_cpu / total_cpu * 100;

    // Read RAM usage
    while (fgets(line, sizeof(line), mem)) {
      if (sscanf(line, "MemTotal: %ld kB", &total_mem) == 1)
        continue;
      if (sscanf(line, "MemFree: %ld kB", &free_mem) == 1)
        break;
    }
    float ram_usage = (1 - (float)free_mem / total_mem) * 100;

    // Print as a one-liner stream
    printf("CPU: %.1f%% | RAM: %.1f%%\n", cpu_usage, ram_usage);

    // Close files and sleep for 1 second
    fclose(cpu);
    fclose(mem);
    sleep(1);
  }
  return 0;
}
