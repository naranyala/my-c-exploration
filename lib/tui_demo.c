#define TUI_IMPLEMENTATION
#include "tui.h"

int main() {
  tui_init();
  tui_clear();

  // Setup colors
  tui_init_color_pair(1, COLOR_RED, COLOR_BLACK);
  tui_init_color_pair(2, COLOR_GREEN, COLOR_BLACK);

  // Draw a box
  tui_draw_box(1, 1, 10, 40);

  // Print colored text
  tui_set_color(1);
  tui_print(3, 5, "Hello, TUI World!");

  tui_set_color(2);
  tui_attr_on(A_BOLD);
  tui_print(5, 5, "Press any key to continue...");
  tui_attr_off(A_BOLD);

  tui_refresh();
  tui_getch();
  tui_end();
  return 0;
}
