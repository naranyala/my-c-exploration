#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

void daemonize(void) {
  pid_t pid = fork();
  if (pid < 0)
    exit(EXIT_FAILURE);
  if (pid > 0)
    exit(EXIT_SUCCESS);

  umask(0);
  sid = setsid();
  if (sid < 0)
    exit(EXIT_FAILURE);

  if (chdir("/") < 0)
    exit(EXIT_FAILURE);

  close(STDIN_FILENO);
  close(STDOUT_FILENO);
  close(STDERR_FILENO);
}
