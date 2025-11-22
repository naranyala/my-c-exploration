#include "raylib.h"
#include "../lib/reactive.h"
#include <stdio.h>


// Computed signal: double the base value
void compute_doubled(Signal *self) {
    Signal *base = self->deps[0];
    self->val.i = get_int(base) * 2;
}

int main() {
    const int screenWidth = 500;
    const int screenHeight = 200;
    InitWindow(screenWidth, screenHeight, "Reactive Counter");
    SetTargetFPS(60);

    // Base counter
    Signal base = signal_int(0);

    // Doubled value (reactive)
    Signal *deps[] = {&base};
    Signal doubled = signal_computed(compute_doubled, deps, 1);

    // Buttons
    const int btnW = 100, btnH = 40;
    Rectangle incBtn = {50, 80, btnW, btnH};
    Rectangle decBtn = {160, 80, btnW, btnH};

    while (!WindowShouldClose()) {
        Vector2 mouse = GetMousePosition();
        bool clicked = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);

        // Increment
        if (CheckCollisionPointRec(mouse, incBtn) && clicked) {
            int newVal = get_int(&base) + 1;
            set_int(&base, newVal);
            printf("Base: %d → Doubled: %d\n", newVal, get_int(&doubled));
            fflush(stdout);
        }

        // Decrement
        if (CheckCollisionPointRec(mouse, decBtn) && clicked) {
            int newVal = get_int(&base) - 1;
            set_int(&base, newVal);
            printf("Base: %d → Doubled: %d\n", newVal, get_int(&doubled));
            fflush(stdout);
        }

        // Render
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Buttons
        DrawRectangleRec(incBtn, LIGHTGRAY);
        DrawRectangleLinesEx(incBtn, 2, BLACK);
        DrawText("INCREMENT", incBtn.x + 10, incBtn.y + 12, 16, BLACK);

        DrawRectangleRec(decBtn, LIGHTGRAY);
        DrawRectangleLinesEx(decBtn, 2, BLACK);
        DrawText("DECREMENT", decBtn.x + 10, decBtn.y + 12, 16, BLACK);

        // Values
        char text1[64];
        sprintf(text1, "Base: %d", get_int(&base));
        DrawText(text1, 300, 50, 24, DARKBLUE);

        char text2[64];
        sprintf(text2, "Doubled: %d", get_int(&doubled));
        DrawText(text2, 300, 90, 24, MAROON);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
