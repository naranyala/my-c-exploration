// demo.c
#define TINY_PHYS_IMPLEMENTATION
#include "../lib/tinyphys.h"
#include "raylib.h"

int main(void) {
    const int screenWidth = 800;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "TinyPhys + Raylib demo");
    SetTargetFPS(60);

    // Physics world
    tp_world world;
    tp_world_init(&world, tp_v2(0.0f, 200.0f)); // gravity downward

    // Ground (static circle)
    int ground = tp_body_add(&world,
        tp_v2(screenWidth/2, screenHeight-50), tp_v2(0,0),
        300.0f, 0.0f, 0.0f, 0.0f, 1);

    // Spawn some balls
    for (int i = 0; i < 5; i++) {
        tp_body_add(&world,
            tp_v2(100 + i*80, 100), tp_v2(0,0),
            20.0f, 1.0f, 0.6f, 0.1f, 0);
    }

    while (!WindowShouldClose()) {
        // Step physics
        tp_step(&world, 1.0f/60.0f);

        // Input: spawn new ball on mouse click
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 m = GetMousePosition();
            tp_body_add(&world,
                tp_v2(m.x, m.y), tp_v2(GetRandomValue(-100,100), -200),
                15.0f, 1.0f, 0.8f, 0.05f, 0);
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Draw all bodies
        for (int i = 0; i < TP_MAX_BODIES; i++) {
            tp_body* b = &world.bodies[i];
            if (!b->alive) continue;

            Color col = b->is_static ? GRAY : BLUE;
            DrawCircleV((Vector2){b->pos.x, b->pos.y}, b->radius, col);
        }

        DrawText("Click to spawn balls", 10, 10, 20, DARKGRAY);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}

