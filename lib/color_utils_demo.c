#include "color_utils.h"
#include "raylib.h"
#include <stdio.h>
#include <time.h>

// Slider helper
float DrawSlider(int posX, int posY, int width, int height, float value,
                 Color color, const char *label) {
  DrawText(label, posX, posY - 20, 20, BLACK);
  DrawRectangle(posX, posY, width, height, LIGHTGRAY);
  int filled = (int)(value * width);
  DrawRectangle(posX, posY, filled, height, color);
  DrawRectangleLines(posX, posY, width, height, DARKGRAY);

  if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
    Vector2 mouse = GetMousePosition();
    if (mouse.x >= posX && mouse.x <= posX + width && mouse.y >= posY &&
        mouse.y <= posY + height) {
      value = (mouse.x - posX) / (float)width;
      if (value < 0)
        value = 0;
      if (value > 1)
        value = 1;
    }
  }
  return value;
}

int main(void) {
  InitWindow(1000, 700, "Rich Color Utils Demo");
  SetTargetFPS(60);
  srand(time(NULL));

  // RGB sliders (normalized 0–1)
  float rVal = 0.2f, gVal = 0.5f, bVal = 0.8f;
  // HSL sliders
  float hVal = 0.6f, sVal = 0.7f, lVal = 0.5f;
  // Manipulation slider
  float lightFactor = 0.0f;

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    // Left panel: RGB
    rVal = DrawSlider(50, 100, 300, 20, rVal, RED, "Red");
    gVal = DrawSlider(50, 150, 300, 20, gVal, GREEN, "Green");
    bVal = DrawSlider(50, 200, 300, 20, bVal, BLUE, "Blue");

    RGB rgb = {(unsigned char)(rVal * 255), (unsigned char)(gVal * 255),
               (unsigned char)(bVal * 255)};
    Color cRGB = (Color){rgb.r, rgb.g, rgb.b, 255};
    DrawRectangle(50, 250, 300, 150, cRGB);

    char hexbuf[8];
    rgb_to_hex(rgb, hexbuf);
    DrawText(TextFormat("RGB Hex: %s", hexbuf), 50, 420, 20, BLACK);

    // Right panel: HSL
    hVal = DrawSlider(500, 100, 300, 20, hVal, ORANGE, "Hue");
    sVal = DrawSlider(500, 150, 300, 20, sVal, PURPLE, "Saturation");
    lVal = DrawSlider(500, 200, 300, 20, lVal, GRAY, "Lightness");

    HSL hsl = {(int)(hVal * 360), (int)(sVal * 100), (int)(lVal * 100)};
    RGB rgbHsl = hsl_to_rgb(hsl);
    Color cHSL = (Color){rgbHsl.r, rgbHsl.g, rgbHsl.b, 255};
    DrawRectangle(500, 250, 300, 150, cHSL);

    rgb_to_hex(rgbHsl, hexbuf);
    DrawText(TextFormat("HSL Hex: %s", hexbuf), 500, 420, 20, BLACK);

    // Manipulation slider
    lightFactor =
        DrawSlider(50, 500, 750, 20, lightFactor, DARKBLUE, "Lighten/Darken");
    RGB manipulated =
        (lightFactor >= 0.5)
            ? lighten(rgb, (lightFactor - 0.5) * 200) // lighten up to +100%
            : darken(rgb, (0.5 - lightFactor) * 200); // darken up to -100%
    Color cManip = (Color){manipulated.r, manipulated.g, manipulated.b, 255};
    DrawRectangle(50, 550, 750, 100, cManip);

    rgb_to_hex(manipulated, hexbuf);
    DrawText(TextFormat("Manipulated Hex: %s", hexbuf), 50, 660, 20, BLACK);

    EndDrawing();
  }

  CloseWindow();
  return 0;
}
