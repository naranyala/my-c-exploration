/* ergo_spawn.h
 * Spawn a child process (Unix-only for now).
 * On Windows, returns -1 and sets errno = ENOSYS.
 */

#ifndef ERGO_SPAWN_H
#define ERGO_SPAWN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Spawn a process with given argv (NULL-terminated).
 * Returns PID on success, -1 on error (check errno).
 */
int ergo_spawn(const char *path, char *const argv[]);

/* Wait for a child process. Returns exit status, or -1 on error. */
int ergo_wait(int pid);

#ifdef __cplusplus
}
#endif

#endif /* ERGO_SPAWN_H */

#ifdef ERGO_SPAWN_IMPLEMENTATION

#if defined(_WIN32) || defined(_WIN64)
  #include <errno.h>
  int ergo_spawn(const char *path, char *const argv[]) {
      (void)path; (void)argv;
      errno = ENOSYS;
      return -1;
  }
  int ergo_wait(int pid) {
      (void)pid;
      errno = ENOSYS;
      return -1;
  }
#else
  #include <sys/wait.h>
  #include <unistd.h>
  #include <errno.h>
  #include <string.h>

  int ergo_spawn(const char *path, char *const argv[]) {
      if (!path || !argv) {
          errno = EINVAL;
          return -1;
      }

      pid_t pid = fork();
      if (pid == 0) {
          /* Child */
          execv(path, argv);
          _exit(127);  /* exec failed */
      } else if (pid > 0) {
          /* Parent */
          return (int)pid;
      } else {
          /* fork failed */
          return -1;
      }
  }

  int ergo_wait(int pid) {
      int status;
      pid_t result = waitpid((pid_t)pid, &status, 0);
      if (result == -1) return -1;
      if (WIFEXITED(status)) {
          return WEXITSTATUS(status);
      }
      return -1;  /* signaled or stopped */
  }
#endif

#endif /* ERGO_SPAWN_IMPLEMENTATION */
