/* better_exec.h */
#ifndef BETTER_EXEC_H
#define BETTER_EXEC_H

#include <stdlib.h>

// Runs a command (argv style). Returns malloc'd string of stdout.
// Caller must free result. Returns NULL on failure.
char* be_exec_read(const char* cmd);

#endif

/* IMPLEMENTATION */
#ifdef BETTER_EXEC_IMPLEMENTATION
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>

char* be_exec_read(const char* cmd) {
    int pipefd[2];
    if (pipe(pipefd) == -1) return NULL;

    pid_t pid = fork();
    if (pid == -1) { close(pipefd[0]); close(pipefd[1]); return NULL; }

    if (pid == 0) {
        // Child
        close(pipefd[0]); // Close read end
        dup2(pipefd[1], STDOUT_FILENO); // Redirect stdout to pipe
        close(pipefd[1]);
        // Use shell to execute command string
        execl("/bin/sh", "sh", "-c", cmd, (char*)NULL);
        exit(127);
    } else {
        // Parent
        close(pipefd[1]); // Close write end
        
        size_t capacity = 256;
        size_t len = 0;
        char* buffer = malloc(capacity);
        char chunk[128];
        ssize_t n;

        while ((n = read(pipefd[0], chunk, sizeof(chunk))) > 0) {
            if (len + n + 1 > capacity) {
                capacity *= 2;
                buffer = realloc(buffer, capacity);
            }
            memcpy(buffer + len, chunk, n);
            len += n;
        }
        if (buffer) buffer[len] = '\0';
        
        close(pipefd[0]);
        waitpid(pid, NULL, 0);
        return buffer;
    }
}
#endif
