
#include "raylib.h"
#include <stdio.h>

typedef struct {
    Vector3 position;
    float radius;
    Color color;
} Ball;

// Create a cube of balls (NxNxN)
int CreateBallCube(Ball *out, int maxBalls, int n, float spacing, float radius) {
    int count = 0;
    float offset = (n - 1) * spacing * 0.5f;   // center the cube at origin

    for (int x = 0; x < n; x++) {
        for (int y = 0; y < n; y++) {
            for (int z = 0; z < n; z++) {
                if (count >= maxBalls) return count;

                out[count].position = (Vector3){
                    x * spacing - offset,
                    y * spacing - offset,
                    z * spacing - offset
                };
                out[count].radius = radius;

                // Example color: gradient by height
                out[count].color = (Color){
                    120 + y*10, 120 + x*10, 200 - z*10, 255
                };

                count++;
            }
        }
    }
    return count;
}

int main(void) {
    const int screenWidth = 1280;
    const int screenHeight = 720;

    InitWindow(screenWidth, screenHeight, "Cube of Balls - C + raylib");
    SetTargetFPS(60);

    // ----------------------------------------------------
    // Generate cube
    // ----------------------------------------------------
    const int N = 6;              // grid size
    const float SPACING = 1.2f;   // distance between spheres
    const float RADIUS = 0.4f;

    Ball balls[600];  // enough for N=8 (512)
    int ballCount = CreateBallCube(balls, 600, N, SPACING, RADIUS);

    Camera3D camera = {0};
    camera.position = (Vector3){8.0f, 8.0f, 8.0f};
    camera.target   = (Vector3){0.0f, 0.0f, 0.0f};
    camera.up       = (Vector3){0.0f, 1.0f, 0.0f};
    camera.fovy     = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    // ----------------------------------------------------
    // Main loop
    // ----------------------------------------------------
    while (!WindowShouldClose()) {
        UpdateCamera(&camera, CAMERA_FREE);

        BeginDrawing();
        ClearBackground(RAYWHITE);

        BeginMode3D(camera);

        // Draw spheres
        for (int i = 0; i < ballCount; i++) {
            DrawSphere(balls[i].position, balls[i].radius, balls[i].color);
        }

        DrawGrid(20, 1.0f);
        EndMode3D();

        DrawText("Cube of balls generated procedurally", 20, 20, 20, DARKGRAY);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
