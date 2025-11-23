/* ergo_proc.h - Ergonomic Subprocess Execution */
#ifndef ERGO_PROC_H
#define ERGO_PROC_H

typedef struct {
    char* output; // Stdout capture
    int exit_code;
    int error;    // 0 = OK, 1 = Fork/Pipe failed
} ergo_cmd_result_t;

// Runs a shell command and captures stdout.
// Caller must free result.output if it is not NULL.
ergo_cmd_result_t ergo_run(const char* cmd);

#endif // ERGO_PROC_H

#ifdef ERGO_IMPLEMENTATION
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

ergo_cmd_result_t ergo_run(const char* cmd) {
    ergo_cmd_result_t res = {0};
    FILE* fp = popen(cmd, "r");
    
    if (fp == NULL) {
        res.error = 1;
        return res;
    }

    // Dynamic buffer growth strategy
    size_t capacity = 128;
    size_t size = 0;
    res.output = (char*)malloc(capacity);
    
    char buffer[128];
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        size_t chunk_len = strlen(buffer);
        if (size + chunk_len + 1 > capacity) {
            capacity *= 2;
            char* new_mem = (char*)realloc(res.output, capacity);
            if (!new_mem) { /* handle OOM */ break; }
            res.output = new_mem;
        }
        strcpy(res.output + size, buffer);
        size += chunk_len;
    }

    res.exit_code = pclose(fp);
    
    // Normalize exit code (WEXITSTATUS equivalent)
    if (res.exit_code != -1) {
        res.exit_code = (res.exit_code >> 8) & 0xFF;
    }
    
    return res;
}
#endif
