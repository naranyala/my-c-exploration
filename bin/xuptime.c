#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#elif __APPLE__
#include <sys/sysctl.h>
#include <sys/time.h>
#else
// Linux, FreeBSD, etc.
#include <unistd.h>
#endif

// Convert seconds to days/hours/minutes
void print_uptime_human(uint64_t total_seconds) {
  if (total_seconds == 0) {
    printf("Uptime: unknown\n");
    return;
  }

  uint64_t secs = total_seconds;
  uint64_t days = secs / (24 * 3600);
  secs %= (24 * 3600);
  uint64_t hours = secs / 3600;
  secs %= 3600;
  uint64_t minutes = secs / 60;

  printf("Uptime: ");
  if (days > 0) {
    printf("%llu day%s, ", (unsigned long long)days, (days == 1) ? "" : "s");
  }
  printf("%llu hour%s, %llu minute%s\n", (unsigned long long)hours,
         (hours == 1) ? "" : "s", (unsigned long long)minutes,
         (minutes == 1) ? "" : "s");
}

uint64_t get_uptime_seconds() {
#ifdef _WIN32
  // GetTickCount64 returns milliseconds since boot
  ULONGLONG ticks = GetTickCount64();
  return (uint64_t)(ticks / 1000);

#elif __APPLE__
  struct timeval boot_time;
  int mib[2] = {CTL_KERN, KERN_BOOTTIME};
  size_t size = sizeof(boot_time);
  if (sysctl(mib, 2, &boot_time, &size, NULL, 0) != 0) {
    return 0;
  }

  // Get current time
  struct timeval now;
  gettimeofday(&now, NULL);

  // Compute uptime in seconds
  time_t uptime = now.tv_sec - boot_time.tv_sec;
  return (uptime > 0) ? (uint64_t)uptime : 0;

#else
  // Linux, BSD, etc. — read /proc/uptime
  FILE *file = fopen("/proc/uptime", "r");
  if (!file) {
    return 0;
  }

  double uptime_sec = 0.0;
  if (fscanf(file, "%lf", &uptime_sec) != 1) {
    fclose(file);
    return 0;
  }
  fclose(file);

  return (uint64_t)uptime_sec;
#endif
}

int main() {
  uint64_t uptime = get_uptime_seconds();
  if (uptime == 0) {
    fprintf(stderr, "Error: Could not determine system uptime.\n");
    return EXIT_FAILURE;
  }

  print_uptime_human(uptime);
  return EXIT_SUCCESS;
}
