/* ergo_signal.h */
#pragma once
#include <signal.h>

typedef void (*sighandler_t)(int);

static inline void signal_handle(int sig, sighandler_t handler) {
    struct sigaction sa = {0};
    sa.sa_handler = handler;
    sa.sa_flags = SA_RESTART;
    sigfillset(&sa.sa_mask);
    sigaction(sig, &sa, NULL);
}

