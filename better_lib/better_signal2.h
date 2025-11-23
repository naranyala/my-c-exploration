/* better_signal.h v1.0 - Install once, enjoy forever */
#ifndef BETTER_SIGNAL_H
#define BETTER_SIGNAL_H
#include <signal.h>
#ifdef BA_HAVE_BACKTRACE
#include <execinfo.h>
#endif

static void bsignal_crash_handler(int sig) {
    const char* name = "UNKNOWN";
    switch(sig) { case SIGSEGV: name="SIGSEGV"; break; case SIGABRT: name="SIGABRT"; break; }
    fprintf(stderr, "\n\033[31mFATAL SIGNAL %d (%s)\033[0m\n", sig, name);
#ifdef BA_HAVE_BACKTRACE
    void* buffer[100];
    int n = backtrace(buffer, 100);
    backtrace_symbols_fd(buffer, n, STDERR_FILENO);
#endif
    _exit(128 + sig);
}

static inline void bsignal_install_crash_handler(void) {
    signal(SIGSEGV, bsignal_crash_handler);
    signal(SIGABRT, bsignal_crash_handler);
    signal(SIGILL,  bsignal_crash_handler);
    signal(SIGFPE,  bsignal_crash_handler);
}

#endif
