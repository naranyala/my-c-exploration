#include "raylib.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib/reactive.h"

// --- Calculator Domain ---

#define MAX_INPUT_CHARS 20

typedef enum { OP_NONE, OP_ADD, OP_SUB, OP_MUL, OP_DIV } Operation;

typedef struct {
  char displayBuffer[MAX_INPUT_CHARS + 1];
  float result;
  float currentVal;
  Operation currentOp;
  bool newNumber;
} CalcState;

// --- Actions ---

void Action_Clear(CalcState *s) {
  strcpy(s->displayBuffer, "0");
  s->result = 0.0f;
  s->currentOp = OP_NONE;
  s->newNumber = true;
}

void Action_Digit(CalcState *s, char digit) {
  if (s->newNumber) {
    strcpy(s->displayBuffer, "");
    s->newNumber = false;
  }
  if (strlen(s->displayBuffer) < MAX_INPUT_CHARS) {
    int len = strlen(s->displayBuffer);
    s->displayBuffer[len] = digit;
    s->displayBuffer[len + 1] = '\0';
  }
}

void Action_Decimal(CalcState *s) {
  if (s->newNumber) {
    strcpy(s->displayBuffer, "0.");
    s->newNumber = false;
  } else if (strchr(s->displayBuffer, '.') == NULL) {
    strcat(s->displayBuffer, ".");
  }
}

void Calculate(CalcState *s) {
  s->currentVal = (float)atof(s->displayBuffer);
  switch (s->currentOp) {
  case OP_ADD:
    s->result += s->currentVal;
    break;
  case OP_SUB:
    s->result -= s->currentVal;
    break;
  case OP_MUL:
    s->result *= s->currentVal;
    break;
  case OP_DIV:
    if (s->currentVal != 0)
      s->result /= s->currentVal;
    break;
  default:
    break;
  }
}

void UpdateDisplayFromResult(CalcState *s) {
  if (s->result == (int)s->result)
    snprintf(s->displayBuffer, MAX_INPUT_CHARS, "%d", (int)s->result);
  else
    snprintf(s->displayBuffer, MAX_INPUT_CHARS, "%.2f", s->result);
}

void Action_Operator(CalcState *s, Operation op) {
  if (!s->newNumber && s->currentOp != OP_NONE) {
    Calculate(s);
    UpdateDisplayFromResult(s);
  } else {
    s->result = (float)atof(s->displayBuffer);
  }
  s->currentOp = op;
  s->newNumber = true;
}

void Action_Equals(CalcState *s) {
  if (s->currentOp != OP_NONE) {
    Calculate(s);
    UpdateDisplayFromResult(s);
    s->currentOp = OP_NONE;
    s->newNumber = true;
  }
}

// --- UI / View ---

// Global layout constants
const int screenWidth = 320;
const int screenHeight = 500;
const int rows = 5;
const int cols = 4;
const int btnWidth = 320 / 4;
const int btnHeight = (500 - 100) / 5;

// Button Labels
const char *btnLabels[20] = {"C", "",  "",  "/", "7", "8", "9", "*", "4", "5",
                             "6", "-", "1", "2", "3", "+", "0", "",  ".", "="};

// The View function just draws the current state
void DrawCalculator(void *data, void *ctx) {
  CalcState *s = (CalcState *)data;
  (void)ctx; // Unused context

  BeginDrawing();
  ClearBackground(RAYWHITE);

  // Draw Display
  DrawRectangle(0, 0, screenWidth, 100, DARKGRAY);
  DrawText(s->displayBuffer, 20, 30, 40, WHITE);

  // Draw Buttons
  for (int i = 0; i < 20; i++) {
    int col = i % cols;
    int row = i / cols;
    int x = col * btnWidth;
    int y = 100 + row * btnHeight;

    const char *label = btnLabels[i];

    // Determine color
    Color btnColor = LIGHTGRAY;
    if (label[0] == 'C')
      btnColor = MAROON;
    else if (label[0] == '=')
      btnColor = DARKGREEN;
    else if (strchr("+-*/", label[0]))
      btnColor = ORANGE;

    DrawRectangle(x, y, btnWidth, btnHeight, btnColor);
    DrawRectangleLines(x, y, btnWidth, btnHeight, DARKGRAY);

    int textWidth = MeasureText(label, 30);
    DrawText(label, x + btnWidth / 2 - textWidth / 2, y + btnHeight / 2 - 15,
             30, BLACK);
  }

  EndDrawing();
}

// Input handling that triggers actions
void HandleInput(ReactiveState *rState) {
  if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
    Vector2 mousePos = GetMousePosition();
    if (mousePos.y > 100) {
      int col = (int)mousePos.x / btnWidth;
      int row = ((int)mousePos.y - 100) / btnHeight;
      int index = row * cols + col;

      if (index >= 0 && index < 20) {
        const char *label = btnLabels[index];
        if (strlen(label) == 0)
          return;

        CalcState *s = (CalcState *)rState->data;
        bool stateChanged = false;

        if (label[0] >= '0' && label[0] <= '9') {
          Action_Digit(s, label[0]);
          stateChanged = true;
        } else if (label[0] == '.') {
          Action_Decimal(s);
          stateChanged = true;
        } else if (label[0] == 'C') {
          Action_Clear(s);
          stateChanged = true;
        } else if (label[0] == '=') {
          Action_Equals(s);
          stateChanged = true;
        } else {
          Operation op = OP_NONE;
          if (label[0] == '+')
            op = OP_ADD;
          else if (label[0] == '-')
            op = OP_SUB;
          else if (label[0] == '*')
            op = OP_MUL;
          else if (label[0] == '/')
            op = OP_DIV;

          if (op != OP_NONE) {
            Action_Operator(s, op);
            stateChanged = true;
          }
        }

        if (stateChanged) {
          SetState(rState);
        }
      }
    }
  }
}

int main(void) {
  InitWindow(screenWidth, screenHeight, "Raylib Reactive Calculator");
  SetTargetFPS(60);

  // Initialize State
  CalcState calcState = {0};
  strcpy(calcState.displayBuffer, "0");
  calcState.newNumber = true;

  ReactiveState appState;
  InitReactiveState(&appState, &calcState);

  // We can subscribe a logger or other systems here if we wanted
  // Subscribe(&appState, LogStateChange, NULL);

  while (!WindowShouldClose()) {
    // Input -> Update State
    HandleInput(&appState);

    // Render (Reactive-ish: The draw function is conceptually an observer of
    // the state, though in Raylib immediate mode we call it every frame anyway.
    // In a true retained mode GUI, 'DrawCalculator' would only be called when
    // Notify() happens.)

    // For this hybrid approach, we just pass the data to the draw function
    // every frame because Raylib clears the screen every frame.
    DrawCalculator(appState.data, NULL);
  }

  CloseWindow();
  return 0;
}
