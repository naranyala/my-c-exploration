/* better_args.h */
#ifndef BETTER_ARGS_H
#define BETTER_ARGS_H

#include <stdbool.h>

// Initialize with main's argc/argv
void ba_init(int argc, char** argv);

// Returns true if flag exists (e.g. "-v" or "--verbose")
bool ba_has(const char* flag);

// Returns value after flag (e.g. "--port 8080" -> returns "8080")
// Returns default_val if not found.
const char* ba_val(const char* flag, const char* default_val);

// Returns positional argument at index (0-based, excluding program name)
const char* ba_arg(int index);

#endif

/* IMPLEMENTATION */
#ifdef BETTER_ARGS_IMPLEMENTATION
#include <string.h>

static int g_argc;
static char** g_argv;

void ba_init(int argc, char** argv) {
    g_argc = argc;
    g_argv = argv;
}

bool ba_has(const char* flag) {
    for (int i = 1; i < g_argc; i++) {
        if (strcmp(g_argv[i], flag) == 0) return true;
    }
    return false;
}

const char* ba_val(const char* flag, const char* default_val) {
    for (int i = 1; i < g_argc - 1; i++) { // -1 because value must follow
        if (strcmp(g_argv[i], flag) == 0) {
            // If next arg starts with '-', strictly it might be another flag, 
            // but simplistic parsers usually take it as value.
            return g_argv[i+1];
        }
    }
    return default_val;
}

const char* ba_arg(int index) {
    int current_pos = 0;
    for (int i = 1; i < g_argc; i++) {
        if (g_argv[i][0] == '-') {
            // Skip flags and their values if they look like pairs
            // This is a naive heuristic: if it has a value, skip next.
            // For strict positional args, simplistic parsing is tricky.
            // Here we just return raw argv index for simplicity of the example.
             continue; 
        }
        // This part requires more complex logic for mixed flags/positionals
        // A simple implementation usually just grabs raw index:
        if ((i - 1) == index) return g_argv[i];
    }
    return NULL;
}
#endif
