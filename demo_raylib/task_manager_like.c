#include "raylib.h"
#include "../lib/reactive.h"
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

#define MAX_PROCESSES 512
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

typedef struct {
    Signal pid;
    Signal name;
    Signal mem_kb;
} ReactiveProcess;

ReactiveProcess processes[MAX_PROCESSES];
int process_count = 0;
Signal total_mem;

// ===== Gather processes into reactive signals =====
void update_process_signals() {
    struct dirent *entry;
    DIR *dp = opendir("/proc");
    if (!dp) return;

    int count = 0;
    while ((entry = readdir(dp)) != NULL && count < MAX_PROCESSES) {
        int pid = atoi(entry->d_name);
        if (pid <= 0) continue;

        char path[256];
        snprintf(path, sizeof(path), "/proc/%d/status", pid);

        FILE *fp = fopen(path, "r");
        if (!fp) continue;

        char line[256], name[256] = {0};
        int mem = 0;

        while (fgets(line, sizeof(line), fp)) {
            if (strncmp(line, "Name:", 5) == 0)
                sscanf(line, "Name:\t%255s", name);
            if (strncmp(line, "VmRSS:", 6) == 0)
                sscanf(line, "VmRSS:\t%d", &mem);
        }
        fclose(fp);

        // Initialize or update signals
        if (count >= process_count) {
            processes[count].pid = signal_int(pid);
            processes[count].name = signal_string(name);
            processes[count].mem_kb = signal_int(mem);
        } else {
            set_int(&processes[count].pid, pid);
            set_string(&processes[count].name, name);
            set_int(&processes[count].mem_kb, mem);
        }
        count++;
    }
    process_count = count;
    closedir(dp);
}

// ===== Computed total memory =====
void compute_total_mem(Signal *self) {
    int sum = 0;
    for (int i = 0; i < process_count; i++)
        sum += get_int(&processes[i].mem_kb);
    set_int(self, sum);
}

void setup_total_mem() {
    Signal *deps[MAX_PROCESSES];
    for (int i = 0; i < process_count; i++)
        deps[i] = &processes[i].mem_kb;

    total_mem = signal_computed(compute_total_mem, deps, process_count);
}

// ===== Main =====
int main(void) {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Raylib Reactive Task Manager");
    SetTargetFPS(60);

    // Initial process scan
    update_process_signals();
    setup_total_mem();

    while (!WindowShouldClose()) {
        // Update process signals periodically
        update_process_signals();
        setup_total_mem();

        BeginDrawing();
        ClearBackground(DARKGRAY);

        // Total memory
        DrawText(TextFormat("Total MEM: %d KB", get_int(&total_mem)), 50, 20, 20, YELLOW);

        // Table header
        DrawText("PID", 50, 50, 18, WHITE);
        DrawText("NAME", 150, 50, 18, WHITE);
        DrawText("MEM(KB)", 400, 50, 18, WHITE);

        // Render each process
        for (int i = 0; i < process_count; i++) {
            int y = 80 + i * 20;
            DrawText(TextFormat("%d", get_int(&processes[i].pid)), 50, y, 16, LIGHTGRAY);
            DrawText(get_string(&processes[i].name), 150, y, 16, LIGHTGRAY);
            DrawText(TextFormat("%d", get_int(&processes[i].mem_kb)), 400, y, 16, LIGHTGRAY);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
