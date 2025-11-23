
#ifndef BETTER_SIGNAL_H
#define BETTER_SIGNAL_H

#include <signal.h>
#include <stdio.h>

typedef void (*signal_handler_t)(int);

static inline void catch_signal(int sig, signal_handler_t handler) {
    struct sigaction sa;
    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(sig, &sa, NULL);
}

static inline void ignore_signal(int sig) {
    struct sigaction sa;
    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(sig, &sa, NULL);
}

#endif // BETTER_SIGNAL_H
