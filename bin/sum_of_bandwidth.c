#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // for sleep()

typedef struct {
  unsigned long rx_bytes;
  unsigned long tx_bytes;
} NetStats;

NetStats get_net_stats() {
  FILE *net = fopen("/proc/net/dev", "r");
  char line[256];
  NetStats stats = {0, 0};

  // Skip the first two lines (header)
  fgets(line, sizeof(line), net);
  fgets(line, sizeof(line), net);

  while (fgets(line, sizeof(line), net)) {
    unsigned long rx, tx;
    char iface[32];
    if (sscanf(line, " %31s %lu %*u %*u %*u %*u %*u %*u %*u %lu", iface, &rx,
               &tx) == 3) {
      stats.rx_bytes += rx;
      stats.tx_bytes += tx;
    }
  }
  fclose(net);
  return stats;
}

int main() {
  NetStats total = {0, 0};

  while (1) {
    NetStats curr = get_net_stats();
    total = curr;

    // Convert bytes to megabytes for readability
    float rx_mb = total.rx_bytes / (1024.0 * 1024.0);
    float tx_mb = total.tx_bytes / (1024.0 * 1024.0);

    // Print the total consumption and current rate
    printf("net-usage: down: %.2f MB | up: %.2f MB\n", rx_mb, tx_mb);

    sleep(1); // Wait 1 second for the next reading
  }
  return 0;
}
