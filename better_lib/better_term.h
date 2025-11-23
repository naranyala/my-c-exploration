/* better_term.h */
#define TERM_RED     "\x1b[31m"
#define TERM_GREEN   "\x1b[32m"
#define TERM_YELLOW  "\x1b[33m"
#define TERM_BLUE    "\x1b[34m"
#define TERM_RESET   "\x1b[0m"
#define TERM_CLEAR   "\x1b[2J\x1b[H"
#define TERM_HIDE_CURSOR "\x1b[?25l"
#define TERM_SHOW_CURSOR "\x1b[?25h"

static inline int bterm_width(void) {
    #ifdef _WIN32
        return 80;  // TODO: proper impl
    #else
        struct winsize w; ioctl(0, TIOCGWINSZ, &w);
        return w.ws_col;
    #endif
}
