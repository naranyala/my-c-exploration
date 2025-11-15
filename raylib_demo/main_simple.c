
#include "raylib.h"

int main(void) {
    InitWindow(800, 450, "raylib starter");
    while (!WindowShouldClose()) {
        BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawText("Raylib works!", 200, 200, 30, BLACK);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
