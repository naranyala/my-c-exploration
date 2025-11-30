#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INPUT_CHARS 20

typedef enum { OP_NONE, OP_ADD, OP_SUB, OP_MUL, OP_DIV } Operation;

int main(void) {
  // Initialization
  const int screenWidth = 320;
  const int screenHeight = 500;

  InitWindow(screenWidth, screenHeight, "Raylib Calculator");

  // Calculator State
  char displayBuffer[MAX_INPUT_CHARS + 1] = "0";
  float result = 0.0f;
  float currentVal = 0.0f;
  Operation currentOp = OP_NONE;
  bool newNumber = true; // Flag to clear display on next digit input

  // Button Layout
  const int rows = 5;
  const int cols = 4;
  const int btnWidth = screenWidth / cols;
  const int btnHeight =
      (screenHeight - 100) / rows; // Top 100px reserved for display

  // Button Labels (Grid 4x5)
  const char *btnLabels[20] = {"C", "",  "",  "/", "7", "8", "9",
                               "*", "4", "5", "6", "-", "1", "2",
                               "3", "+", "0", "",  ".", "="};

  SetTargetFPS(60);

  // Main game loop
  while (!WindowShouldClose()) {
    // Update
    //----------------------------------------------------------------------------------
    Vector2 mousePos = GetMousePosition();
    bool mousePressed = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

    if (mousePressed) {
      // Check if click is in button area (below y=100)
      if (mousePos.y > 100) {
        int col = (int)mousePos.x / btnWidth;
        int row = ((int)mousePos.y - 100) / btnHeight;
        int index = row * cols + col;

        if (index >= 0 && index < 20) {
          const char *label = btnLabels[index];

          // Ignore empty buttons
          if (strlen(label) > 0) {
            // --- Digit Logic ---
            if (label[0] >= '0' && label[0] <= '9') {
              if (newNumber) {
                strcpy(displayBuffer, "");
                newNumber = false;
              }
              if (strlen(displayBuffer) < MAX_INPUT_CHARS) {
                strcat(displayBuffer, label);
              }
            }
            // --- Decimal Point ---
            else if (label[0] == '.') {
              if (newNumber) {
                strcpy(displayBuffer, "0.");
                newNumber = false;
              } else if (strchr(displayBuffer, '.') == NULL) {
                strcat(displayBuffer, ".");
              }
            }
            // --- Clear ---
            else if (label[0] == 'C') {
              strcpy(displayBuffer, "0");
              result = 0.0f;
              currentOp = OP_NONE;
              newNumber = true;
            }
            // --- Equals ---
            else if (label[0] == '=') {
              currentVal = (float)atof(displayBuffer);
              if (currentOp != OP_NONE) {
                switch (currentOp) {
                case OP_ADD:
                  result += currentVal;
                  break;
                case OP_SUB:
                  result -= currentVal;
                  break;
                case OP_MUL:
                  result *= currentVal;
                  break;
                case OP_DIV:
                  if (currentVal != 0)
                    result /= currentVal;
                  break;
                default:
                  break;
                }
                // Format result to remove trailing zeros if integer
                if (result == (int)result)
                  snprintf(displayBuffer, MAX_INPUT_CHARS, "%d", (int)result);
                else
                  snprintf(displayBuffer, MAX_INPUT_CHARS, "%.2f", result);

                currentOp = OP_NONE;
                newNumber = true;
              }
            }
            // --- Operators ---
            else {
              // If we were chaining operations (e.g. 1+2+), calculate previous
              // first
              if (!newNumber && currentOp != OP_NONE) {
                currentVal = (float)atof(displayBuffer);
                switch (currentOp) {
                case OP_ADD:
                  result += currentVal;
                  break;
                case OP_SUB:
                  result -= currentVal;
                  break;
                case OP_MUL:
                  result *= currentVal;
                  break;
                case OP_DIV:
                  if (currentVal != 0)
                    result /= currentVal;
                  break;
                default:
                  break;
                }
                snprintf(displayBuffer, MAX_INPUT_CHARS, "%.2f", result);
              } else {
                result = (float)atof(displayBuffer);
              }

              if (label[0] == '+')
                currentOp = OP_ADD;
              else if (label[0] == '-')
                currentOp = OP_SUB;
              else if (label[0] == '*')
                currentOp = OP_MUL;
              else if (label[0] == '/')
                currentOp = OP_DIV;

              newNumber = true;
            }
          }
        }
      }
    }
    //----------------------------------------------------------------------------------

    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();

    ClearBackground(RAYWHITE);

    // Draw Display Area
    DrawRectangle(0, 0, screenWidth, 100, DARKGRAY);
    DrawText(displayBuffer, 20, 30, 40, WHITE);

    // Draw Buttons
    for (int i = 0; i < 20; i++) {
      int col = i % cols;
      int row = i / cols;
      int x = col * btnWidth;
      int y = 100 + row * btnHeight;

      // Determine button color
      Color btnColor = LIGHTGRAY;
      if (btnLabels[i][0] == 'C')
        btnColor = MAROON;
      else if (btnLabels[i][0] == '=')
        btnColor = DARKGREEN;
      else if (strchr("+-*/", btnLabels[i][0]))
        btnColor = ORANGE;

      // Draw Button Background
      DrawRectangle(x, y, btnWidth, btnHeight, btnColor);
      DrawRectangleLines(x, y, btnWidth, btnHeight, DARKGRAY); // Outline

      // Draw Button Text
      int textWidth = MeasureText(btnLabels[i], 30);
      DrawText(btnLabels[i], x + btnWidth / 2 - textWidth / 2,
               y + btnHeight / 2 - 15, 30, BLACK);
    }

    EndDrawing();
    //----------------------------------------------------------------------------------
  }

  // De-Initialization
  CloseWindow();

  return 0;
}