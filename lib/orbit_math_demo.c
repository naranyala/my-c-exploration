#include "raylib.h"
#include "orbit_math.h"

int main(void) {
    const int screenWidth = 800;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "Orbit Demo with orbit_math.h");

    OrbitState orbit = init_orbit_state(screenWidth, screenHeight, 0.0f);

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        // --- Update ---
        update_orbit_state(&orbit, 1.0f); // delta_time = 1 frame step

        // Planet position in 3D
        Vector3 planet3D = get_planet_position(&orbit, ORBIT_RADIUS, orbit.angle);

        // Project to 2D
        Projected2D planet2D = project_3d_to_2d(&orbit, planet3D, 300.0f);

        // --- Draw ---
        BeginDrawing();
        ClearBackground(BLACK);

        // Draw orbit circle (approximate in 2D)
        DrawCircleLines((int)orbit.centerX, (int)orbit.centerY, ORBIT_RADIUS, DARKGRAY);

        // Draw planet
        DrawCircle((int)planet2D.x_screen, (int)planet2D.y_screen, 10 * planet2D.scale, SKYBLUE);

        // Debug info
        DrawText(TextFormat("Angle: %.2f", orbit.angle), 10, 10, 20, RAYWHITE);
        DrawText(TextFormat("Scale: %.2f", planet2D.scale), 10, 40, 20, RAYWHITE);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}

