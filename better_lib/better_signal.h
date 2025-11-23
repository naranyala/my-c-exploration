
/* better_signal.h */

#ifndef BETTER_SIGNAL_H
#define BETTER_SIGNAL_H

#include <signal.h>

typedef void (*bsig_handler)(int sig);

static bsig_handler bsig_table[32];

static void bsig_trampoline(int sig) {
    if (sig >= 0 && sig < 32 && bsig_table[sig])
        bsig_table[sig](sig);
}

static inline int bsig_register(int sig, bsig_handler fn) {
    if (sig < 0 || sig >= 32) return 0;
    bsig_table[sig] = fn;
    return signal(sig, bsig_trampoline) != SIG_ERR;
}

#endif
