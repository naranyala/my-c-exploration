/* ergo_term.h — terminal color/cursor helpers + raw mode */
#ifndef ERGO_TERM_H
#define ERGO_TERM_H

#ifdef __cplusplus
extern "C" {
#endif

/* Foreground colors */
#define ERGO_T_RED     "\x1b[31m"
#define ERGO_T_GREEN   "\x1b[32m"
#define ERGO_T_YELLOW  "\x1b[33m"
#define ERGO_T_BLUE    "\x1b[34m"
#define ERGO_T_RESET   "\x1b[0m"

/* Cursor movement */
#define ergo_term_clear()       printf("\x1b[2J")
#define ergo_term_home()        printf("\x1b[H")
#define ergo_term_moveto(r,c)   printf("\x1b[%d;%dH", (r), (c))

/* Raw mode control */
int ergo_term_raw(int enable);

#ifdef __cplusplus
}
#endif
#endif /* ERGO_TERM_H */

#ifdef ERGO_TERM_IMPLEMENTATION
#include <stdio.h>

#ifdef _WIN32
#include <windows.h>
int ergo_term_raw(int enable){
    (void)enable;
    return -1; /* not implemented for now */
}
#else
#include <termios.h>
static struct termios oldt;
int ergo_term_raw(int enable){
    if (enable){
        struct termios newt;
        tcgetattr(0, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(0, TCSANOW, &newt);
        return 0;
    } else {
        tcsetattr(0, TCSANOW, &oldt);
        return 0;
    }
}
#endif
#endif /* ERGO_TERM_IMPLEMENTATION */

