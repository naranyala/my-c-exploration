#ifndef TUI_H
#define TUI_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>
#include <stdio.h>

// Initialize the TUI environment
void tui_init();

// Refresh the screen to display updates
void tui_refresh();

// End the TUI environment and restore terminal settings
void tui_end();

// Print formatted text at a specific position (y, x)
void tui_print(int y, int x, const char *fmt, ...);

// Get user input as a string at a specific position (y, x)
void tui_input(int y, int x, char *buffer, int max_len);

// Get a single character input without echoing
int tui_getch();

// Clear the screen
void tui_clear();

// Get terminal dimensions (rows and columns)
void tui_get_size(int *rows, int *cols);

// Set cursor visibility (0 = invisible, 1 = normal, 2 = very visible)
void tui_set_cursor(int visibility);

// Initialize color pair (pair_id, foreground, background)
// pair_id: 1-255, colors: COLOR_BLACK, COLOR_RED, COLOR_GREEN, COLOR_YELLOW,
//                         COLOR_BLUE, COLOR_MAGENTA, COLOR_CYAN, COLOR_WHITE
void tui_init_color_pair(int pair_id, int fg, int bg);

// Activate a color pair for subsequent text
void tui_set_color(int pair_id);

// Reset to default colors
void tui_reset_color();

// Enable/disable text attributes (A_BOLD, A_UNDERLINE, A_REVERSE, A_BLINK,
// A_DIM)
void tui_attr_on(int attrs);
void tui_attr_off(int attrs);

// Draw a box at position (y, x) with given dimensions
void tui_draw_box(int y, int x, int height, int width);

// Set non-blocking input mode (timeout in milliseconds, -1 for blocking)
void tui_set_timeout(int timeout);

// Move cursor to position (y, x)
void tui_move(int y, int x);

#ifdef TUI_IMPLEMENTATION

#include <ncurses.h>

void tui_init() {
  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);

  // Initialize colors if terminal supports it
  if (has_colors()) {
    start_color();
    use_default_colors();
  }
}

void tui_refresh() { refresh(); }

void tui_end() { endwin(); }

void tui_print(int y, int x, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  move(y, x);
  vw_printw(stdscr, fmt, args);
  va_end(args);
}

void tui_input(int y, int x, char *buffer, int max_len) {
  if (buffer && max_len > 0) {
    move(y, x);
    echo();
    getnstr(buffer, max_len - 1);
    noecho();
    buffer[max_len - 1] = '\0';
  }
}

int tui_getch() { return getch(); }

void tui_clear() { clear(); }

void tui_get_size(int *rows, int *cols) {
  if (rows && cols) {
    getmaxyx(stdscr, *rows, *cols);
  }
}

void tui_set_cursor(int visibility) { curs_set(visibility); }

void tui_init_color_pair(int pair_id, int fg, int bg) {
  if (has_colors() && pair_id > 0 && pair_id < 256) {
    init_pair(pair_id, fg, bg);
  }
}

void tui_set_color(int pair_id) {
  if (has_colors() && pair_id > 0 && pair_id < 256) {
    attron(COLOR_PAIR(pair_id));
  }
}

void tui_reset_color() {
  if (has_colors()) {
    attrset(A_NORMAL);
  }
}

void tui_attr_on(int attrs) { attron(attrs); }

void tui_attr_off(int attrs) { attroff(attrs); }

void tui_draw_box(int y, int x, int height, int width) {
  if (height < 2 || width < 2)
    return;

  // Draw corners
  mvaddch(y, x, ACS_ULCORNER);
  mvaddch(y, x + width - 1, ACS_URCORNER);
  mvaddch(y + height - 1, x, ACS_LLCORNER);
  mvaddch(y + height - 1, x + width - 1, ACS_LRCORNER);

  // Draw horizontal lines
  for (int i = 1; i < width - 1; i++) {
    mvaddch(y, x + i, ACS_HLINE);
    mvaddch(y + height - 1, x + i, ACS_HLINE);
  }

  // Draw vertical lines
  for (int i = 1; i < height - 1; i++) {
    mvaddch(y + i, x, ACS_VLINE);
    mvaddch(y + i, x + width - 1, ACS_VLINE);
  }
}

void tui_set_timeout(int timeout) { timeout(timeout); }

void tui_move(int y, int x) { move(y, x); }

#endif // TUI_IMPLEMENTATION

#ifdef __cplusplus
}
#endif

#endif // TUI_H
