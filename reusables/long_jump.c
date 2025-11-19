#include <setjmp.h>
#include <signal.h>

static jmp_buf env;
static void alarm_handler(int sig) { longjmp(env, 1); }

#define WITH_TIMEOUT(seconds, code)                                            \
  do {                                                                         \
    if (setjmp(env) == 0) {                                                    \
      signal(SIGALRM, alarm_handler);                                          \
      alarm(seconds);                                                          \
      code;                                                                    \
      alarm(0);                                                                \
    }                                                                          \
  } while (0)
