#ifndef ERGO_SIGNAL_H
#define ERGO_SIGNAL_H

#include <signal.h>
#include <stdio.h>

// Register a simple signal handler
static inline void ergo_signal_handle(int sig, void (*handler)(int)) {
    struct sigaction sa;
    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(sig, &sa, NULL) < 0) {
        perror("ergo_signal: sigaction failed");
    }
}

#endif // ERGO_SIGNAL_H

