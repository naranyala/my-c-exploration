#include "../lib/reactive.h"
#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Signals
Signal input;
Signal accumulator;
Signal operation;
Signal display;

// ----- Computed Display -----
void compute_display(Signal *self) {
  char buf[256];
  const char *op = get_string(self->deps[2]);

  if (strlen(op) > 0) {
    snprintf(buf, sizeof(buf), "%g %s %s", get_double(self->deps[1]), op,
             get_string(self->deps[0]));
  } else {
    snprintf(buf, sizeof(buf), "%s", get_string(self->deps[0]));
  }

  set_string(self, buf);
}

// ----- Calculator Logic -----
void doOperation() {
  const char *op = get_string(&operation);

  // Only perform operation if one is set
  if (strlen(op) == 0) {
    // First number entry
    set_double(&accumulator, atof(get_string(&input)));
    set_string(&input, "0");
    return;
  }

  double a = get_double(&accumulator);
  double b = atof(get_string(&input));

  if (strcmp(op, "+") == 0)
    a += b;
  else if (strcmp(op, "-") == 0)
    a -= b;
  else if (strcmp(op, "x") == 0)
    a *= b;
  else if (strcmp(op, "/") == 0 && b != 0)
    a /= b;

  set_double(&accumulator, a);
  set_string(&input, "0");
}

void pressDigit(char c) {
  char buf[256];
  const char *curr = get_string(&input);

  if (strcmp(curr, "0") == 0)
    snprintf(buf, sizeof(buf), "%c", c);
  else
    snprintf(buf, sizeof(buf), "%s%c", curr, c);

  set_string(&input, buf);
}

void pressOp(const char *op) {
  doOperation();
  set_string(&operation, op);
}

void pressEq() {
  doOperation();
  set_string(&operation, "");

  char buf[256];
  snprintf(buf, sizeof(buf), "%g", get_double(&accumulator));
  set_string(&input, buf);
}

void pressClear() {
  set_string(&input, "0");
  set_double(&accumulator, 0);
  set_string(&operation, "");
}

// ----- UI Button -----
int Button(Rectangle r, const char *label) {
  Vector2 mp = GetMousePosition();
  int hover = CheckCollisionPointRec(mp, r);

  DrawRectangleRec(r, hover ? LIGHTGRAY : RAYWHITE);
  DrawRectangleLinesEx(r, 2, DARKGRAY);

  int tw = MeasureText(label, 30);
  DrawText(label, r.x + (r.width - tw) / 2, r.y + 20, 30, BLACK);

  return hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

int main() {
  InitWindow(400, 600, "Reactive Calculator");
  SetTargetFPS(60);

  // Initialize signals
  input = signal_string("0");
  accumulator = signal_double(0.0);
  operation = signal_string("");

  // Register signals for auto-propagation
  register_signal(&input);
  register_signal(&accumulator);
  register_signal(&operation);

  // Create computed display signal
  Signal *deps[] = {&input, &accumulator, &operation};
  display = signal_computed(compute_display, deps, 3);
  register_signal(&display);

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    // Display (automatically updated!)
    DrawRectangle(20, 20, 360, 100, RAYWHITE);
    DrawRectangleLines(20, 20, 360, 100, GRAY);
    DrawText(get_string(&display), 30, 50, 40, BLACK);

    // Buttons
    const char *L[4][4] = {{"7", "8", "9", "/"},
                           {"4", "5", "6", "x"},
                           {"1", "2", "3", "-"},
                           {"C", "0", "=", "+"}};

    float bx = 20, by = 140, w = 90, h = 90;
    for (int r = 0; r < 4; r++) {
      for (int c = 0; c < 4; c++) {
        Rectangle rect = {bx + c * w, by + r * h, w - 10, h - 10};
        const char *lab = L[r][c];

        if (Button(rect, lab)) {
          if (lab[0] >= '0' && lab[0] <= '9') {
            pressDigit(lab[0]);
          } else if (strcmp(lab, "C") == 0) {
            pressClear();
          } else if (strcmp(lab, "=") == 0) {
            pressEq();
          } else {
            pressOp(lab);
          }
        }
      }
    }

    EndDrawing();
  }

  CloseWindow();
  return 0;
}
