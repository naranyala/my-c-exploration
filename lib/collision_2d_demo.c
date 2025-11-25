/*********************************************************************
 * 2D Collision Demo with raylib + collision.h
 * 
 * Features:
 *   - Draggable circle (follows mouse when clicked)
 *   - Static AABB obstacles
 *   - Visual feedback: red = collision, green = no collision
 * 
 * Build:
 *   gcc demo.c -o demo -lraylib -lm -lpthread -ldl -lX11 -lGL
 *   (or use raylib's recommended build method)
 *********************************************************************/

#define COLLISION_IMPLEMENTATION
#include "collision.h"   // your collision library
#include "raylib.h"

#include <stdio.h>
#include <stdbool.h>

int main(void) {
    const int screenWidth = 800;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "2D Collision Demo - collision.h + raylib");

    SetTargetFPS(60);

    // Circle controlled by mouse
    Circle player = {{0, 0}, 30.0f};
    bool dragging = false;

    // Static obstacles (AABB2)
    AABB2 obstacles[] = {
        {{100, 100}, {200, 200}},
        {{300, 250}, {450, 350}},
        {{600, 100}, {700, 180}},
        {{500, 400}, {750, 500}}
    };
    const int obstacleCount = sizeof(obstacles) / sizeof(obstacles[0]);

    while (!WindowShouldClose()) {
        Vector2 mousePos = GetMousePosition();

        // Handle dragging
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            // Check if mouse is inside player circle
            float dx = mousePos.x - player.center.x;
            float dy = mousePos.y - player.center.y;
            if (dx*dx + dy*dy <= player.radius * player.radius) {
                dragging = true;
            }
        }
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            dragging = false;
        }

        if (dragging) {
            player.center.x = mousePos.x;
            player.center.y = mousePos.y;
        }

        // Check collisions
        bool collided = false;
        for (int i = 0; i < obstacleCount; i++) {
            if (collide_aabb2_circle(&obstacles[i], &player)) {
                collided = true;
                break;
            }
        }

        // Drawing
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Draw obstacles
        for (int i = 0; i < obstacleCount; i++) {
            DrawRectangle(
                (int)obstacles[i].min.x,
                (int)obstacles[i].min.y,
                (int)(obstacles[i].max.x - obstacles[i].min.x),
                (int)(obstacles[i].max.y - obstacles[i].min.y),
                collided ? RED : DARKGRAY
            );
        }

        // Draw player circle
        Color playerColor = collided ? RED : GREEN;
        DrawCircle((int)player.center.x, (int)player.center.y, player.radius, playerColor);
        DrawCircleLines((int)player.center.x, (int)player.center.y, player.radius, DARKGREEN);

        // Instructions
        DrawText("Click and drag the green circle!", 10, 10, 20, DARKGRAY);
        DrawText("It turns RED on collision with any gray box.", 10, 40, 20, DARKGRAY);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
