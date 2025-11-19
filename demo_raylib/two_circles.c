#include "raylib.h"

int main(void) {
  const int screenWidth = 800;
  const int screenHeight = 450;

  InitWindow(screenWidth, screenHeight,
             "Circles Connected by Center Line (Line Below)");
  SetTargetFPS(60);

  float radius = 40.0f;
  Vector2 c1 = {screenWidth / 2 - 100, screenHeight * 0.6f};
  Vector2 c2 = {screenWidth / 2 + 100, screenHeight * 0.6f};

  bool dragging1 = false;
  bool dragging2 = false;
  Vector2 offset1 = {0};
  Vector2 offset2 = {0};

  while (!WindowShouldClose()) {
    Vector2 mouse = GetMousePosition();

    // Drag start
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
      if (CheckCollisionPointCircle(mouse, c1, radius)) {
        dragging1 = true;
        offset1 = (Vector2){mouse.x - c1.x, mouse.y - c1.y};
      } else if (CheckCollisionPointCircle(mouse, c2, radius)) {
        dragging2 = true;
        offset2 = (Vector2){mouse.x - c2.x, mouse.y - c2.y};
      }
    }

    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
      dragging1 = dragging2 = false;
    }

    if (dragging1) {
      c1.x = mouse.x - offset1.x;
      c1.y = mouse.y - offset1.y;
    }
    if (dragging2) {
      c2.x = mouse.x - offset2.x;
      c2.y = mouse.y - offset2.y;
    }

    BeginDrawing();
    ClearBackground(RAYWHITE);

    // === IMPORTANT: Draw the connecting line FIRST (so it's behind the
    // circles) ===
    DrawLineEx(c1, c2, 12.0f, DARKGRAY); // thick outer line
    DrawLineEx(c1, c2, 8.0f, BLACK);     // sharp inner line

    // Draw circles on top
    DrawCircleV(c1, radius, SKYBLUE);
    DrawCircleV(c2, radius, PINK);

    // Optional: white outline so circles look clean even with thick line
    // underneath
    DrawCircleLines((int)c1.x, (int)c1.y, radius, WHITE);
    DrawCircleLines((int)c2.x, (int)c2.y, radius, WHITE);

    EndDrawing();
  }

  CloseWindow();
  return 0;
}
