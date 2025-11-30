// calculator_demo.c
#define ANO_REACTIVE_RAYLIB_EXAMPLE
#include "ano_reactivity.h"
#include "raylib.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h> // for atof
#include <string.h>

// ------------------------------------------------------------
// Reactive signals
// ------------------------------------------------------------
RSIGNAL(char *) display;  // current display text (e.g. "123.45" or "0")
RSIGNAL(double) operand1; // first operand
RSIGNAL(double) operand2; // second operand (when operator is active)
RSIGNAL(char) op;         // current operator: '+','-','*','/', or 0
RSIGNAL(bool)
new_input; // true after '=' or operator → next digits replace display

RCOMPUTED(char *) display_value; // Note: Use char* for computed string

// Helper to set computed string value
#define rc_set_value_str(comp_ptr, newval)                                     \
  do {                                                                         \
    if (!(comp_ptr)->initialized ||                                            \
        strcmp((newval), ((comp_ptr)->cached)) != 0) {                         \
      if ((comp_ptr)->cached)                                                  \
        ANO_REACTIVE_FREE((comp_ptr)->cached);                                 \
      size_t _len = strlen(newval) + 1;                                        \
      (comp_ptr)->cached = r_memdup(newval, _len);                             \
      r_signalbase_mark_subs_dirty(&(comp_ptr)->base);                         \
    }                                                                          \
    (comp_ptr)->initialized = true;                                            \
  } while (0)

static void compute_display_value(REffect *self, void *ctx) {
  char buf[32];
  double val_to_show;

  if (rs_get(&new_input)) {
    val_to_show = (rs_get(&op) == 0) ? rs_get(&operand1) : rs_get(&operand2);
  } else {
    val_to_show = rs_get(&operand1);
  }

  snprintf(buf, sizeof(buf), "%.10g", val_to_show);
  if (buf[0] == '\0' || strcmp(buf, "nan") == 0 || strcmp(buf, "inf") == 0) {
    strcpy(buf, "0");
  }

  rc_set_value_str(&display_value, buf);
}

// ------------------------------------------------------------
// Helper functions
// ------------------------------------------------------------
static void append_digit(int digit) {
  char current[32];
  strncpy(current, rs_get(&display), sizeof(current) - 1);
  current[sizeof(current) - 1] = '\0';

  bool starting_new = rs_get(&new_input);
  rs_set_force(&new_input, false);

  if (starting_new) {
    strcpy(current, "0");
  }

  if (strcmp(current, "0") == 0 && digit != 0) {
    current[0] = '\0'; // Replace leading zero
  }

  char digchar = '0' + digit;
  size_t len = strlen(current);
  if (len < 20) { // Safety limit
    current[len] = digchar;
    current[len + 1] = '\0';
  }

  double val = atof(current);
  if (rs_get(&op) == 0) {
    rs_set(&operand1, val);
  } else {
    rs_set(&operand2, val);
  }
}

static void append_dot() {
  char current[32];
  strncpy(current, rs_get(&display), sizeof(current) - 1);
  current[sizeof(current) - 1] = '\0';

  if (rs_get(&new_input)) {
    rs_set_force(&new_input, false);
    strcpy(current, "0");
  }

  if (strchr(current, '.') == NULL) {
    size_t len = strlen(current);
    if (len < 20) {
      current[len] = '.';
      current[len + 1] = '\0';
    }
    double val = atof(current);
    if (rs_get(&op) == 0) {
      rs_set(&operand1, val);
    } else {
      rs_set(&operand2, val);
    }
  }
}

static void set_operator(char new_op) {
  rs_set_force(&op, new_op);
  rs_set_force(&new_input, true);
}

static void calculate() {
  double a = rs_get(&operand1);
  double b = (rs_get(&op) == 0) ? 0.0 : rs_get(&operand2);
  char oper = rs_get(&op);
  double result = a;

  switch (oper) {
  case '+':
    result = a + b;
    break;
  case '-':
    result = a - b;
    break;
  case '*':
    result = a * b;
    break;
  case '/':
    if (b != 0.0)
      result = a / b;
    else
      result = 0.0; // Avoid div-by-zero
    break;
  }

  rs_set(&operand1, result);
  rs_set_force(&operand2, 0.0);
  rs_set_force(&op, 0);
  rs_set_force(&new_input, true);
}

static void clear_all() {
  rs_set(&operand1, 0.0);
  rs_set(&operand2, 0.0);
  rs_set_force(&op, 0);
  rs_set_force(&new_input, true);
}

// ------------------------------------------------------------
// Main
// ------------------------------------------------------------
int main(void) {
  const int screenWidth = 600;
  const int screenHeight = 800;

  InitWindow(screenWidth, screenHeight, "Reactive Raylib Calculator");
  SetTargetFPS(60);

  // Initialize reactive state
  rs_init_str(&display, "0");
  rs_init(&operand1, 0.0);
  rs_init(&operand2, 0.0);
  rs_init(&op, 0);
  rs_init(&new_input, true);

  rc_init(&display_value, compute_display_value, NULL);

  // Flush once to establish initial dependencies
  r_flush();

  while (!WindowShouldClose()) {
    // ----------------------------------------------------
    // Input handling (fixed keys)
    // ----------------------------------------------------
    // Digits
    if (IsKeyPressed(KEY_ZERO) || IsKeyPressed(KEY_KP_0))
      append_digit(0);
    if (IsKeyPressed(KEY_ONE) || IsKeyPressed(KEY_KP_1))
      append_digit(1);
    if (IsKeyPressed(KEY_TWO) || IsKeyPressed(KEY_KP_2))
      append_digit(2);
    if (IsKeyPressed(KEY_THREE) || IsKeyPressed(KEY_KP_3))
      append_digit(3);
    if (IsKeyPressed(KEY_FOUR) || IsKeyPressed(KEY_KP_4))
      append_digit(4);
    if (IsKeyPressed(KEY_FIVE) || IsKeyPressed(KEY_KP_5))
      append_digit(5);
    if (IsKeyPressed(KEY_SIX) || IsKeyPressed(KEY_KP_6))
      append_digit(6);
    if (IsKeyPressed(KEY_SEVEN) || IsKeyPressed(KEY_KP_7))
      append_digit(7);
    if (IsKeyPressed(KEY_EIGHT) || IsKeyPressed(KEY_KP_8))
      append_digit(8);
    if (IsKeyPressed(KEY_NINE) || IsKeyPressed(KEY_KP_9))
      append_digit(9);

    if (IsKeyPressed(KEY_PERIOD) || IsKeyPressed(KEY_KP_DECIMAL))
      append_dot();

    // Operators (fixed: use numpad equivalents)
    if (IsKeyPressed(KEY_KP_ADD))
      set_operator('+');
    if (IsKeyPressed(KEY_MINUS) || IsKeyPressed(KEY_KP_SUBTRACT))
      set_operator('-');
    if (IsKeyPressed(KEY_SLASH) || IsKeyPressed(KEY_KP_DIVIDE))
      set_operator('/');
    if (IsKeyPressed(KEY_KP_MULTIPLY))
      set_operator('*');

    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER) ||
        IsKeyPressed(KEY_EQUAL)) {
      set_operator('=');
      calculate();
    }
    if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_C))
      clear_all();

    // Mouse/touch buttons (unchanged)
    Vector2 mouse = GetMousePosition();

    // ----------------------------------------------------
    // UI layout (simple grid)
    // ----------------------------------------------------
    BeginDrawing();
    ClearBackground(RAYWHITE);

    // Display (fixed: use rs_get for char*)
    const char *disp = rc_get(&display_value);
    DrawRectangle(30, 40, 500, 120, DARKGRAY);
    DrawText(disp ? disp : "0", 45, 85, 48, LIGHTGRAY);

    // Buttons 4x4 grid
    const int btn_w = 80;
    const int btn_h = 80;
    const int start_x = 40;
    const int start_y = 180;
    const char *labels[4][5] = {{"7", "8", "9", "/"},
                                {"4", "5", "6", "*"},
                                {"1", "2", "3", "-"},
                                {"C", "0", ".", "+", "="}};

    for (int y = 0; y < 4; y++) {
      for (int x = 0; x < 4; x++) {
        int bx = start_x + x * (btn_w + 20);
        int by = start_y + y * (btn_h + 20);
        Rectangle rec = {(float)bx, (float)by, (float)btn_w, (float)btn_h};
        bool pressed = CheckCollisionPointRec(mouse, rec) &&
                       IsMouseButtonPressed(MOUSE_LEFT_BUTTON);

        Color col = LIGHTGRAY;
        if (CheckCollisionPointRec(mouse, rec))
          col = GRAY;
        if (pressed)
          col = DARKGRAY;

        DrawRectangleRec(rec, col);
        DrawText(labels[y][x], bx + 28, by + 20, 40, BLACK);

        if (pressed) {
          char c = labels[y][x][0];
          if (c >= '0' && c <= '9')
            append_digit(c - '0');
          else if (c == '.')
            append_dot();
          else if (c == 'C')
            clear_all();
          else if (c == '+' || c == '-' || c == '*' || c == '/')
            set_operator(c);
        }
      }
    }

    // Big = button on the right
    Rectangle eq_rec = {(float)(start_x + 4 * (btn_w + 20)), (float)start_y,
                        (float)btn_w, (float)(4 * (btn_h + 20) - 20)};
    bool eq_pressed = CheckCollisionPointRec(mouse, eq_rec) &&
                      IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    Color eq_col = GREEN;
    if (CheckCollisionPointRec(mouse, eq_rec))
      eq_col = DARKGREEN;
    if (eq_pressed)
      eq_col = (Color){0, 150, 0, 255};

    DrawRectangleRec(eq_rec, eq_col);
    DrawText("=", (int)eq_rec.x + 20, (int)eq_rec.y + 120, 60, WHITE);

    if (eq_pressed) {
      calculate();
    }

    // Flush any scheduled reactive updates
    r_flush();

    EndDrawing();
  }

  // Cleanup (fixed for strings)
  rc_free(&display_value); // Frees internal cached string
  rs_free_str(&display);
  rs_free(&operand1);
  rs_free(&operand2);
  rs_free(&op);
  rs_free(&new_input);

  r_shutdown();

  CloseWindow();
  return 0;
}
