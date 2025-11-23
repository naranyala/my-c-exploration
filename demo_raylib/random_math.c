#include "raylib.h"
#include "../better_lib/better_math.h"

int main(void) {
    InitWindow(800, 600, "better_math + Raylib Demo");
    SetTargetFPS(60);

    // Ball properties
    double ball_x = 100.0;
    double ball_y = 300.0;
    double target_x = 100.0;
    double target_y = 300.0;

    // Pulsing circle
    double pulse_time = 0.0;

    // Rotating arrow
    double arrow_angle = 0.0;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        // Click to set target
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            target_x = GetMouseX();
            target_y = GetMouseY();
        }

        // Smooth follow using lerp
        ball_x = lerp(ball_x, target_x, 5.0 * dt);
        ball_y = lerp(ball_y, target_y, 5.0 * dt);

        // Pulse effect using smoothstep (oscillate 0 -> 1 -> 0)
        pulse_time += dt;
        double pulse_t = (sin(pulse_time * 3.0) + 1.0) / 2.0;  // 0 to 1
        double pulse_radius = lerp(20.0, 35.0, smoothstep(0.0, 1.0, pulse_t));

        // Rotate arrow (keep angle normalized)
        arrow_angle += 90.0 * dt;  // 90 degrees per second
        arrow_angle = normalize_deg(arrow_angle);

        // Health bar demo (mouse X controls "damage")
        double health_pct = remap(GetMouseX(), 0, 800, 0.0, 1.0);
        health_pct = clamp(health_pct, 0.0, 1.0);

        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Draw target marker
        DrawCircleLines((int)target_x, (int)target_y, 15, LIGHTGRAY);

        // Draw pulsing ball (follows mouse clicks)
        DrawCircle((int)ball_x, (int)ball_y, (float)pulse_radius, BLUE);

        // Draw rotating arrow
        float arrow_cx = 650, arrow_cy = 150;
        float arrow_len = 50;
        float rad = (float)deg2rad(arrow_angle);
        DrawLine(
            arrow_cx, arrow_cy,
            arrow_cx + cos(rad) * arrow_len,
            arrow_cy + sin(rad) * arrow_len,
            RED
        );
        DrawCircle(arrow_cx, arrow_cy, 5, RED);
        DrawText(TextFormat("%.0f deg", arrow_angle), 610, 180, 16, DARKGRAY);

        // Draw health bar
        int bar_width = 200;
        int bar_height = 25;
        int bar_x = 300, bar_y = 50;
        DrawRectangle(bar_x, bar_y, bar_width, bar_height, LIGHTGRAY);
        DrawRectangle(bar_x, bar_y, (int)(bar_width * health_pct), bar_height, RED);
        DrawRectangleLines(bar_x, bar_y, bar_width, bar_height, DARKGRAY);
        DrawText(TextFormat("HP: %.0f%%", health_pct * 100), bar_x + 70, bar_y + 5, 16, WHITE);

        // Instructions
        DrawText("Click anywhere - ball follows smoothly (lerp)", 10, 550, 16, DARKGRAY);
        DrawText("Move mouse left/right - health bar (remap + clamp)", 10, 570, 16, DARKGRAY);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
