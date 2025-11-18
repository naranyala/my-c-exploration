#include "raylib.h"
#include "reactive.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
    #include <windows.h>
    #include <psapi.h>
#else
    #include <unistd.h>
    #include <sys/sysinfo.h>
#endif

#define HISTORY_SIZE 60

// Signals
Signal cpu_percent;
Signal ram_percent;
Signal ram_used_mb;
Signal ram_total_mb;

// Computed displays
Signal cpu_text;
Signal ram_text;

// History for graphs
float cpu_history[HISTORY_SIZE] = {0};
float ram_history[HISTORY_SIZE] = {0};
int history_index = 0;

// ----- Computed Text Signals -----
void compute_cpu_text(Signal *self) {
    char buf[64];
    snprintf(buf, sizeof(buf), "CPU: %.1f%%", get_double(self->deps[0]));
    set_string(self, buf);
}

void compute_ram_text(Signal *self) {
    char buf[128];
    snprintf(buf, sizeof(buf), "RAM: %.1f%% (%.0f/%.0f MB)",
             get_double(self->deps[0]),
             get_double(self->deps[1]),
             get_double(self->deps[2]));
    set_string(self, buf);
}

// ----- System Monitoring -----
#ifdef _WIN32
static ULARGE_INTEGER last_cpu_idle, last_cpu_kernel, last_cpu_user;

void init_cpu_monitor() {
    FILETIME idle, kernel, user;
    GetSystemTimes(&idle, &kernel, &user);
    last_cpu_idle.LowPart = idle.dwLowDateTime;
    last_cpu_idle.HighPart = idle.dwHighDateTime;
    last_cpu_kernel.LowPart = kernel.dwLowDateTime;
    last_cpu_kernel.HighPart = kernel.dwHighDateTime;
    last_cpu_user.LowPart = user.dwLowDateTime;
    last_cpu_user.HighPart = user.dwHighDateTime;
}

double get_cpu_usage() {
    FILETIME idle, kernel, user;
    GetSystemTimes(&idle, &kernel, &user);
    
    ULARGE_INTEGER curr_idle, curr_kernel, curr_user;
    curr_idle.LowPart = idle.dwLowDateTime;
    curr_idle.HighPart = idle.dwHighDateTime;
    curr_kernel.LowPart = kernel.dwLowDateTime;
    curr_kernel.HighPart = kernel.dwHighDateTime;
    curr_user.LowPart = user.dwLowDateTime;
    curr_user.HighPart = user.dwHighDateTime;
    
    ULONGLONG idle_diff = curr_idle.QuadPart - last_cpu_idle.QuadPart;
    ULONGLONG kernel_diff = curr_kernel.QuadPart - last_cpu_kernel.QuadPart;
    ULONGLONG user_diff = curr_user.QuadPart - last_cpu_user.QuadPart;
    ULONGLONG total = kernel_diff + user_diff;
    
    last_cpu_idle = curr_idle;
    last_cpu_kernel = curr_kernel;
    last_cpu_user = curr_user;
    
    if (total == 0) return 0.0;
    return (double)(total - idle_diff) * 100.0 / total;
}

void get_ram_usage(double *percent, double *used_mb, double *total_mb) {
    MEMORYSTATUSEX mem;
    mem.dwLength = sizeof(mem);
    GlobalMemoryStatusEx(&mem);
    
    *total_mb = mem.ullTotalPhys / (1024.0 * 1024.0);
    *used_mb = (mem.ullTotalPhys - mem.ullAvailPhys) / (1024.0 * 1024.0);
    *percent = mem.dwMemoryLoad;
}

#else  // Linux/Unix
static unsigned long long last_total = 0, last_idle = 0;

void init_cpu_monitor() {
    FILE *fp = fopen("/proc/stat", "r");
    if (fp) {
        char buf[256];
        fgets(buf, sizeof(buf), fp);
        unsigned long long user, nice, system, idle;
        sscanf(buf, "cpu %llu %llu %llu %llu", &user, &nice, &system, &idle);
        last_total = user + nice + system + idle;
        last_idle = idle;
        fclose(fp);
    }
}

double get_cpu_usage() {
    FILE *fp = fopen("/proc/stat", "r");
    if (!fp) return 0.0;
    
    char buf[256];
    fgets(buf, sizeof(buf), fp);
    unsigned long long user, nice, system, idle;
    sscanf(buf, "cpu %llu %llu %llu %llu", &user, &nice, &system, &idle);
    fclose(fp);
    
    unsigned long long total = user + nice + system + idle;
    unsigned long long total_diff = total - last_total;
    unsigned long long idle_diff = idle - last_idle;
    
    last_total = total;
    last_idle = idle;
    
    if (total_diff == 0) return 0.0;
    return (double)(total_diff - idle_diff) * 100.0 / total_diff;
}

void get_ram_usage(double *percent, double *used_mb, double *total_mb) {
    struct sysinfo info;
    sysinfo(&info);
    
    *total_mb = info.totalram / (1024.0 * 1024.0);
    *used_mb = (info.totalram - info.freeram) / (1024.0 * 1024.0);
    *percent = (*used_mb / *total_mb) * 100.0;
}
#endif

void update_system_stats() {
    double cpu = get_cpu_usage();
    double ram_pct, ram_used, ram_total;
    get_ram_usage(&ram_pct, &ram_used, &ram_total);
    
    // Update signals (triggers reactive updates!)
    set_double(&cpu_percent, cpu);
    set_double(&ram_percent, ram_pct);
    set_double(&ram_used_mb, ram_used);
    set_double(&ram_total_mb, ram_total);
    
    // Update history
    cpu_history[history_index] = cpu;
    ram_history[history_index] = ram_pct;
    history_index = (history_index + 1) % HISTORY_SIZE;
}

// ----- Drawing -----
void draw_graph(int x, int y, int width, int height, float *data, Color color) {
    // Background
    DrawRectangle(x, y, width, height, Fade(DARKGRAY, 0.3f));
    DrawRectangleLines(x, y, width, height, GRAY);
    
    // Grid lines
    for (int i = 0; i <= 4; i++) {
        int line_y = y + (height * i / 4);
        DrawLine(x, line_y, x + width, line_y, Fade(GRAY, 0.3f));
    }
    
    // Graph data
    float bar_width = (float)width / HISTORY_SIZE;
    for (int i = 0; i < HISTORY_SIZE; i++) {
        int idx = (history_index + i) % HISTORY_SIZE;
        float value = data[idx];
        int bar_height = (int)((value / 100.0f) * height);
        int bar_x = x + (int)(i * bar_width);
        int bar_y = y + height - bar_height;
        
        DrawRectangle(bar_x, bar_y, (int)bar_width + 1, bar_height, color);
    }
    
    // 100% line
    DrawLine(x, y, x + width, y, RED);
}

void draw_bar(int x, int y, int width, int height, double percent, Color color) {
    // Background
    DrawRectangle(x, y, width, height, Fade(DARKGRAY, 0.3f));
    DrawRectangleLines(x, y, width, height, GRAY);
    
    // Filled portion
    int fill_width = (int)((percent / 100.0) * width);
    DrawRectangle(x, y, fill_width, height, color);
    
    // Percentage text
    char buf[16];
    snprintf(buf, sizeof(buf), "%.1f%%", percent);
    int tw = MeasureText(buf, 20);
    DrawText(buf, x + (width - tw) / 2, y + (height - 20) / 2, 20, WHITE);
}

int main() {
    InitWindow(500, 600, "System Monitor");
    SetTargetFPS(60);
    
    // Initialize signals
    cpu_percent = signal_double(0.0);
    ram_percent = signal_double(0.0);
    ram_used_mb = signal_double(0.0);
    ram_total_mb = signal_double(0.0);
    
    register_signal(&cpu_percent);
    register_signal(&ram_percent);
    register_signal(&ram_used_mb);
    register_signal(&ram_total_mb);
    
    // Create computed text signals
    Signal *cpu_deps[] = {&cpu_percent};
    cpu_text = signal_computed(compute_cpu_text, cpu_deps, 1);
    register_signal(&cpu_text);
    
    Signal *ram_deps[] = {&ram_percent, &ram_used_mb, &ram_total_mb};
    ram_text = signal_computed(compute_ram_text, ram_deps, 3);
    register_signal(&ram_text);
    
    // Initialize CPU monitoring
    init_cpu_monitor();
    
    float update_timer = 0.0f;
    float update_interval = 1.0f;  // Update every second
    
    while (!WindowShouldClose()) {
        // Update stats periodically
        update_timer += GetFrameTime();
        if (update_timer >= update_interval) {
            update_timer = 0.0f;
            update_system_stats();
        }
        
        BeginDrawing();
        ClearBackground(BLACK);
        
        // Title
        DrawText("SYSTEM MONITOR", 20, 20, 30, WHITE);
        
        // CPU Section
        DrawText(get_string(&cpu_text), 20, 70, 24, SKYBLUE);
        draw_bar(20, 100, 460, 40, get_double(&cpu_percent), SKYBLUE);
        draw_graph(20, 150, 460, 150, cpu_history, SKYBLUE);
        
        // RAM Section
        DrawText(get_string(&ram_text), 20, 320, 24, LIME);
        draw_bar(20, 350, 460, 40, get_double(&ram_percent), LIME);
        draw_graph(20, 400, 460, 150, ram_history, LIME);
        
        // Footer
        DrawText("Updates every second", 20, 570, 16, GRAY);
        
        EndDrawing();
    }
    
    CloseWindow();
    return 0;
}
