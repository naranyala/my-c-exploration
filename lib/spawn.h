
#ifndef SPAWN_H
#define SPAWN_H

typedef struct {
    const char *path;
    char *const *argv;
    char *const *env;
    const char *cwd;
} spawn_opts;

int spawn_run(const spawn_opts *opts, int wait);

#ifdef SPAWN_IMPLEMENTATION

#if _WIN32
#include <windows.h>

int spawn_run(const spawn_opts *opts, int wait) {
    STARTUPINFOA si = { .cb = sizeof(si) };
    PROCESS_INFORMATION pi;

    char cmd[1024] = {0};
    for (int i=0; opts->argv[i]; i++) {
        strcat(cmd, opts->argv[i]);
        strcat(cmd, " ");
    }

    BOOL ok = CreateProcessA(
        opts->path, cmd, NULL, NULL, FALSE, 0,
        opts->env, opts->cwd, &si, &pi
    );
    if (!ok) return 0;
    if (wait) WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return 1;
}

#else
#include <unistd.h>
#include <sys/wait.h>

int spawn_run(const spawn_opts *opts, int wait) {
    pid_t pid = fork();
    if (pid < 0) return 0;
    if (pid == 0) {
        if (opts->cwd) chdir(opts->cwd);
        execve(opts->path, opts->argv, opts->env);
        _exit(127);
    }
    if (wait) waitpid(pid, NULL, 0);
    return 1;
}

#endif
#endif // SPAWN_IMPLEMENTATION

#endif
