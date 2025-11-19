#define TUI_IMPLEMENTATION
#include "tui.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define MAX_CPU_SAMPLES 50
#define UPDATE_INTERVAL 1000 // milliseconds
#define MAX_PROCESSES 20

// Color pairs
#define COLOR_HEADER 1
#define COLOR_CPU_BAR 2
#define COLOR_MEM_BAR 3
#define COLOR_LABEL 4
#define COLOR_WARNING 5
#define COLOR_CRITICAL 6

typedef struct {
  unsigned long long user, nice, system, idle, iowait, irq, softirq;
} CPUStats;

typedef struct {
  unsigned long total, free, available, buffers, cached;
} MemStats;

typedef struct {
  int pid;
  char name[256];
  float cpu_percent;
  unsigned long mem_kb;
} Process;

// CPU history for graph
float cpu_history[MAX_CPU_SAMPLES];
int history_index = 0;

void read_cpu_stats(CPUStats *stats) {
  FILE *fp = fopen("/proc/stat", "r");
  if (!fp)
    return;

  fscanf(fp, "cpu %llu %llu %llu %llu %llu %llu %llu", &stats->user,
         &stats->nice, &stats->system, &stats->idle, &stats->iowait,
         &stats->irq, &stats->softirq);

  fclose(fp);
}

float calculate_cpu_usage(CPUStats *prev, CPUStats *curr) {
  unsigned long long prev_idle = prev->idle + prev->iowait;
  unsigned long long curr_idle = curr->idle + curr->iowait;

  unsigned long long prev_total = prev->user + prev->nice + prev->system +
                                  prev->idle + prev->iowait + prev->irq +
                                  prev->softirq;
  unsigned long long curr_total = curr->user + curr->nice + curr->system +
                                  curr->idle + curr->iowait + curr->irq +
                                  curr->softirq;

  unsigned long long total_diff = curr_total - prev_total;
  unsigned long long idle_diff = curr_idle - prev_idle;

  if (total_diff == 0)
    return 0.0f;

  return (float)(total_diff - idle_diff) * 100.0f / total_diff;
}

void read_mem_stats(MemStats *stats) {
  FILE *fp = fopen("/proc/meminfo", "r");
  if (!fp)
    return;

  char line[256];
  while (fgets(line, sizeof(line), fp)) {
    if (sscanf(line, "MemTotal: %lu kB", &stats->total) == 1)
      continue;
    if (sscanf(line, "MemFree: %lu kB", &stats->free) == 1)
      continue;
    if (sscanf(line, "MemAvailable: %lu kB", &stats->available) == 1)
      continue;
    if (sscanf(line, "Buffers: %lu kB", &stats->buffers) == 1)
      continue;
    if (sscanf(line, "Cached: %lu kB", &stats->cached) == 1)
      continue;
  }

  fclose(fp);
}

int get_top_processes(Process *processes, int max_count) {
  FILE *fp = popen("ps aux --sort=-%cpu | head -n 21 | tail -n 20", "r");
  if (!fp)
    return 0;

  char line[512];
  int count = 0;

  while (fgets(line, sizeof(line), fp) && count < max_count) {
    char user[64], cmd[256];
    float cpu;
    unsigned long mem;
    int pid;

    if (sscanf(line, "%s %d %f %*f %*s %*s %*s %*s %*s %*s %[^\n]", user, &pid,
               &cpu, cmd) >= 3) {

      // Get memory in KB from ps command
      sscanf(line, "%*s %*d %*f %*f %*s %lu", &mem);

      processes[count].pid = pid;
      processes[count].cpu_percent = cpu;
      processes[count].mem_kb = mem;

      // Truncate long command names
      strncpy(processes[count].name, cmd, 30);
      processes[count].name[30] = '\0';

      count++;
    }
  }

  pclose(fp);
  return count;
}

void draw_progress_bar(int y, int x, int width, float percent, int color) {
  tui_set_color(color);
  tui_print(y, x, "[");

  int filled = (int)(width * percent / 100.0f);

  for (int i = 0; i < width; i++) {
    if (i < filled) {
      tui_attr_on(A_REVERSE);
      tui_print(y, x + 1 + i, " ");
      tui_attr_off(A_REVERSE);
    } else {
      tui_print(y, x + 1 + i, " ");
    }
  }

  tui_print(y, x + width + 1, "]");
  tui_reset_color();
}

void draw_cpu_graph(int y, int x, int width, int height) {
  tui_set_color(COLOR_CPU_BAR);
  tui_draw_box(y, x, height, width);

  tui_attr_on(A_BOLD);
  tui_print(y, x + 2, " CPU History ");
  tui_attr_off(A_BOLD);

  // Draw graph
  int graph_width = width - 4;
  int graph_height = height - 3;

  for (int i = 0; i < graph_width && i < MAX_CPU_SAMPLES; i++) {
    int idx =
        (history_index - graph_width + i + MAX_CPU_SAMPLES) % MAX_CPU_SAMPLES;
    float val = cpu_history[idx];

    int bar_height = (int)(val * graph_height / 100.0f);

    for (int j = 0; j < bar_height && j < graph_height; j++) {
      tui_print(y + height - 2 - j, x + 2 + i, "|");
    }
  }

  tui_reset_color();
}

void draw_monitor(float cpu_percent, MemStats *mem, Process *processes,
                  int proc_count) {
  int rows, cols;
  tui_get_size(&rows, &cols);

  tui_clear();

  // Header
  tui_set_color(COLOR_HEADER);
  tui_attr_on(A_BOLD | A_REVERSE);
  char header[256];
  snprintf(header, sizeof(header), " System Monitor ");
  tui_print(0, (cols - strlen(header)) / 2, "%s", header);
  tui_attr_off(A_BOLD | A_REVERSE);

  time_t now = time(NULL);
  char time_str[64];
  strftime(time_str, sizeof(time_str), "%H:%M:%S", localtime(&now));
  tui_print(0, cols - 12, " %s ", time_str);
  tui_reset_color();

  int current_y = 2;

  // CPU Section
  tui_set_color(COLOR_LABEL);
  tui_attr_on(A_BOLD);
  tui_print(current_y, 2, "CPU Usage:");
  tui_attr_off(A_BOLD);
  tui_reset_color();

  int bar_color = COLOR_CPU_BAR;
  if (cpu_percent > 90.0f)
    bar_color = COLOR_CRITICAL;
  else if (cpu_percent > 70.0f)
    bar_color = COLOR_WARNING;

  draw_progress_bar(current_y, 15, cols - 35, cpu_percent, bar_color);
  tui_print(current_y, cols - 15, "%5.1f%%", cpu_percent);

  current_y += 2;

  // Memory Section
  unsigned long used_mem = mem->total - mem->available;
  float mem_percent = (float)used_mem * 100.0f / mem->total;

  tui_set_color(COLOR_LABEL);
  tui_attr_on(A_BOLD);
  tui_print(current_y, 2, "Memory:");
  tui_attr_off(A_BOLD);
  tui_reset_color();

  bar_color = COLOR_MEM_BAR;
  if (mem_percent > 90.0f)
    bar_color = COLOR_CRITICAL;
  else if (mem_percent > 70.0f)
    bar_color = COLOR_WARNING;

  draw_progress_bar(current_y, 15, cols - 35, mem_percent, bar_color);
  tui_print(current_y, cols - 15, "%5.1f%%", mem_percent);

  current_y++;
  tui_print(current_y, 15, "Used: %.1f GB / Total: %.1f GB",
            used_mem / 1024.0f / 1024.0f, mem->total / 1024.0f / 1024.0f);

  current_y += 2;

  // CPU Graph
  if (rows > 20) {
    draw_cpu_graph(current_y, 2, cols - 4, 10);
    current_y += 11;
  }

  // Top Processes
  tui_set_color(COLOR_LABEL);
  tui_attr_on(A_BOLD);
  tui_print(current_y, 2, "Top Processes by CPU:");
  tui_attr_off(A_BOLD);
  tui_reset_color();

  current_y++;
  tui_print(current_y, 2, "%-8s %-6s %-10s %s", "PID", "CPU%", "MEM(MB)",
            "COMMAND");
  current_y++;

  for (int i = 0; i < proc_count && current_y < rows - 2; i++) {
    tui_print(current_y, 2, "%-8d %5.1f%% %9.1f  %.30s", processes[i].pid,
              processes[i].cpu_percent, processes[i].mem_kb / 1024.0f,
              processes[i].name);
    current_y++;
  }

  // Footer
  tui_set_color(COLOR_HEADER);
  tui_print(rows - 1, 2, " Press 'q' to quit | Updates every second ");
  tui_reset_color();

  tui_refresh();
}

int main() {
  tui_init();
  tui_set_cursor(0);

  // Initialize colors
  tui_init_color_pair(COLOR_HEADER, COLOR_BLACK, COLOR_CYAN);
  tui_init_color_pair(COLOR_CPU_BAR, COLOR_GREEN, -1);
  tui_init_color_pair(COLOR_MEM_BAR, COLOR_BLUE, -1);
  tui_init_color_pair(COLOR_LABEL, COLOR_CYAN, -1);
  tui_init_color_pair(COLOR_WARNING, COLOR_YELLOW, -1);
  tui_init_color_pair(COLOR_CRITICAL, COLOR_RED, -1);

  CPUStats prev_stats, curr_stats;
  MemStats mem_stats;
  Process processes[MAX_PROCESSES];

  // Initialize CPU history
  memset(cpu_history, 0, sizeof(cpu_history));

  // Initial CPU reading
  read_cpu_stats(&prev_stats);

  tui_set_timeout(UPDATE_INTERVAL);

  int running = 1;
  while (running) {
    // Read current stats
    read_cpu_stats(&curr_stats);
    read_mem_stats(&mem_stats);

    float cpu_percent = calculate_cpu_usage(&prev_stats, &curr_stats);

    // Update CPU history
    cpu_history[history_index] = cpu_percent;
    history_index = (history_index + 1) % MAX_CPU_SAMPLES;

    // Get top processes
    int proc_count = get_top_processes(processes, MAX_PROCESSES);

    // Draw the monitor
    draw_monitor(cpu_percent, &mem_stats, processes, proc_count);

    // Check for quit
    int key = tui_getch();
    if (key == 'q' || key == 'Q' || key == 27) {
      running = 0;
    }

    // Update previous stats
    prev_stats = curr_stats;
  }

  tui_end();
  printf("System monitor terminated.\n");

  return 0;
}
