#ifdef _WIN32
#include <windows.h>
#define sleep_ms(x) Sleep(x)
#else
#include <time.h>
#include <unistd.h>
#define sleep_ms(x)                                                            \
  do {                                                                         \
    struct timespec ts = {0, (x) * 1000000L};                                  \
    nanosleep(&ts, NULL);                                                      \
  } while (0)
#endif
