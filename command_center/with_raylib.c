#include "raylib.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdarg.h> // Required for va_list, va_start, etc.

// --- Configuration ---
#define CMD_MAX 256
#define LINE_MAX 256
#define HISTORY_LINES 15 // Number of lines to store/display in the console history
#define FONT_SIZE 20
#define MARGIN 20

// --- Data Structures ---
typedef void (*CommandHandler)(const char *args);

typedef struct {
    const char *name;
    const char *description;
    CommandHandler handler;
} Command;

// --- State Variables ---
char cmd_buffer[CMD_MAX] = {0};
int cursor = 0;
int should_quit = 0;

// Console History (Circular Buffer)
char history[HISTORY_LINES][LINE_MAX];
int history_idx = 0; // Index of the next line to be written

// --- Forward Declarations ---
void cmd_help(const char *args);
void cmd_clear(const char *args);
void cmd_echo(const char *args);
void cmd_time(const char *args);
void cmd_quit(const char *args);
void log_to_history(const char *format, ...); // New logging function

// --- Command Registry ---
Command commands[] = {
    {"help",  "Show available commands",      cmd_help},
    {"clear", "Clear command history",        cmd_clear}, // Changed behavior
    {"echo",  "Echo back the given text",     cmd_echo},
    {"time",  "Show current system time",     cmd_time},
    {"quit",  "Exit the program",             cmd_quit},
};
int command_count = sizeof(commands)/sizeof(Command);

// --- History & Logging ---

/**
 * @brief Logs a formatted string to the console history buffer, handling wrapping.
 * @param format The format string.
 * @param ... Arguments for the format string.
 */
void log_to_history(const char *format, ...) {
    char temp_line[LINE_MAX];
    va_list args;

    va_start(args, format);
    vsnprintf(temp_line, LINE_MAX, format, args);
    va_end(args);

    // Copy the resulting string into the circular buffer
    strncpy(history[history_idx], temp_line, LINE_MAX - 1);
    history[history_idx][LINE_MAX - 1] = '\0'; // Ensure null-termination

    // Move to the next line index (circular buffer logic)
    history_idx = (history_idx + 1) % HISTORY_LINES;

    // Zero out the next line to be safe
    history[history_idx][0] = '\0';
}

// --- Command Implementations ---

void cmd_help(const char *args) {
    log_to_history("");
    log_to_history("--- Available Commands ---");
    for (int i = 0; i < command_count; i++) {
        log_to_history("> %s: %s", commands[i].name, commands[i].description);
    }
    log_to_history("");
}

void cmd_clear(const char *args) {
    // Clear the history buffer completely
    for (int i = 0; i < HISTORY_LINES; i++) {
        history[i][0] = '\0';
    }
    history_idx = 0;
    
    // Clear the command input bar
    cmd_buffer[0] = 0;
    cursor = 0;

    log_to_history("Console history cleared.");
}

void cmd_echo(const char *args) {
    log_to_history("Echo: %s", args);
}

void cmd_time(const char *args) {
    time_t now = time(NULL);
    char *time_str = ctime(&now); 
    
    // Copy and remove newline
    char temp_output[LINE_MAX];
    strncpy(temp_output, time_str, LINE_MAX);
    temp_output[LINE_MAX - 1] = '\0'; 
    
    // Remove newline from ctime's output
    temp_output[strcspn(temp_output, "\n")] = 0;
    
    log_to_history("Time: %s", temp_output);
}

void cmd_quit(const char *args) {
    should_quit = 1;
}

// --- Command Dispatcher ---

void execute_command(const char *cmd) {
    char name[64] = {0};
    // Use the full available space for arguments
    char args[CMD_MAX - 64] = {0}; 
    
    // Log the input command first
    log_to_history("$ %s", cmd);

    // sscanf to split command name and arguments
    if (sscanf(cmd, "%63s %191[^\n]", name, args) < 1) {
        // Command was empty or only whitespace
        return; 
    }

    // Check if the command name is not empty
    if (name[0] == '\0') {
        return;
    }
    
    int found = 0;
    for (int i = 0; i < command_count; i++) {
        if (strcmp(name, commands[i].name) == 0) {
            commands[i].handler(args);
            found = 1;
            break;
        }
    }
    
    if (!found) {
        log_to_history("Error: Unknown command '%s'. Type 'help'.", name);
    }
}

// --- Input Handling ---

void update_text_input() {
    int key = GetCharPressed();
    while (key > 0) {
        // Check for printable ASCII (32-126) and buffer space
        if (key >= 32 && key <= 126 && cursor < CMD_MAX - 1) { 
            cmd_buffer[cursor++] = (char)key;
            cmd_buffer[cursor] = 0;
        }
        key = GetCharPressed();
    }

    if (IsKeyPressed(KEY_BACKSPACE) && cursor > 0) {
        cmd_buffer[--cursor] = 0;
    }
}

// --- Main Program ---

int main() {
    // Determine window height based on history lines and command bar
    const int window_height = HISTORY_LINES * (FONT_SIZE + 4) + FONT_SIZE + MARGIN * 4;
    const int window_width = 800;
    
    InitWindow(window_width, window_height, "Raylib Terminal Emulator"); 
    SetTargetFPS(60);
    
    // Load a custom, fixed-width font if you have one, or use the default
    // Font terminal_font = LoadFontEx("path/to/monospace.ttf", FONT_SIZE, 0, 0); 
    
    // Initial message
    log_to_history("Raylib Terminal Emulator initialized.");
    log_to_history("Type 'help' to see available commands.");
    log_to_history("");

    while (!WindowShouldClose() && !should_quit) {
        update_text_input();

        if (IsKeyPressed(KEY_ENTER)) {
            if (cmd_buffer[0] != '\0') { 
                execute_command(cmd_buffer);
                // Reset input
                cmd_buffer[0] = 0;
                cursor = 0;
            } else {
                // Log an empty line if only Enter was pressed
                log_to_history("$ ");
            }
        }

        BeginDrawing();
        // Background color (Dark Green/Blue-Gray)
        ClearBackground((Color){20, 30, 30, 255}); 

        // --- Draw Console History ---
        int current_y = MARGIN;
        
        // Loop through the circular buffer to draw lines
        for (int i = 0; i < HISTORY_LINES; i++) {
            // Calculate the index in the circular array, starting from the oldest line
            int index = (history_idx + i) % HISTORY_LINES;
            
            // Draw the line (Use a different color for history)
            DrawText(history[index], MARGIN, current_y, FONT_SIZE, RAYWHITE);
            
            current_y += FONT_SIZE + 4; // Move down for the next line
        }
        
        // Horizontal separator line (Optional)
        DrawLine(MARGIN, current_y, window_width - MARGIN, current_y, DARKGRAY);
        current_y += MARGIN / 2;


        // --- Draw Command Prompt ---
        // Draw the prompt text (e.g., '> ')
        const char* prompt = "> ";
        int prompt_width = MeasureText(prompt, FONT_SIZE);
        DrawText(prompt, MARGIN, current_y, FONT_SIZE, GREEN);

        // Draw the command buffer content
        int input_x = MARGIN + prompt_width;
        DrawText(cmd_buffer, input_x, current_y, FONT_SIZE, LIME);
        
        // Draw the blinking cursor
        if (((GetTime() * 2) - (int)(GetTime() * 2)) < 0.5) {
            int text_width = MeasureText(cmd_buffer, FONT_SIZE);
            DrawText("|", input_x + text_width, current_y, FONT_SIZE, LIME);
        }

        EndDrawing();
    }

    // UnloadFont(terminal_font); // If you loaded a font
    CloseWindow();
    return 0;
}
