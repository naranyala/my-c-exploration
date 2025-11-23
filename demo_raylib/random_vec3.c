#include "raylib.h"
#include "../better_lib/better_math.h"
#include "../better_lib/better_vec.h"

#define MAX_CUBES 10

// Convert our Vec3 to Raylib's Vector3
#define TO_RL(v) (Vector3){v.x, v.y, v.z}

typedef struct {
    Vec3 pos;
    Vec3 vel;
    Vec3 rot;
    Color color;
} Cube;

int main(void) {
    InitWindow(1000, 700, "better_vec 3D + Raylib Demo");
    SetTargetFPS(60);

    // Camera orbit
    float cam_distance = 20.0f;
    float cam_yaw = 0.0f;
    float cam_pitch = 0.4f;

    // Player sphere
    Vec3 player_pos = vec3_zero();
    Vec3 player_vel = vec3_zero();
    float player_speed = 10.0f;
    float friction = 3.0f;

    // Floating cubes
    Cube cubes[MAX_CUBES];
    for (int i = 0; i < MAX_CUBES; i++) {
        cubes[i].pos = vec3(
            (float)GetRandomValue(-15, 15),
            (float)GetRandomValue(1, 8),
            (float)GetRandomValue(-15, 15)
        );
        cubes[i].vel = vec3_zero();
        cubes[i].rot = vec3_zero();
        cubes[i].color = (Color){
            GetRandomValue(100, 255),
            GetRandomValue(100, 255),
            GetRandomValue(100, 255),
            255
        };
    }

    // Orbiting satellite
    float orbit_angle = 0.0f;
    float orbit_radius = 4.0f;
    float orbit_height = 2.0f;

    DisableCursor();

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        /* ── Camera Control ───────────────────────────────────── */
        Vector2 mouse = GetMouseDelta();
        cam_yaw -= mouse.x * 0.003f;
        cam_pitch -= mouse.y * 0.003f;
        cam_pitch = clamp(cam_pitch, -1.2f, 1.2f);

        // Zoom
        cam_distance -= GetMouseWheelMove() * 2.0f;
        cam_distance = clamp(cam_distance, 5.0f, 50.0f);

        // Calculate camera position (spherical coords)
        Vec3 cam_offset = vec3(
            cosf(cam_pitch) * sinf(cam_yaw),
            sinf(cam_pitch),
            cosf(cam_pitch) * cosf(cam_yaw)
        );
        Vec3 cam_pos = vec3_add(player_pos, vec3_scale(cam_offset, cam_distance));

        /* ── Player Movement ──────────────────────────────────── */
        // Get forward/right vectors from camera (ignore Y)
        Vec3 cam_forward = vec3_norm(vec3(-sinf(cam_yaw), 0, -cosf(cam_yaw)));
        Vec3 cam_right = vec3_norm(vec3_cross(cam_forward, vec3_up()));

        Vec3 input = vec3_zero();
        if (IsKeyDown(KEY_W)) input = vec3_add(input, cam_forward);
        if (IsKeyDown(KEY_S)) input = vec3_sub(input, cam_forward);
        if (IsKeyDown(KEY_D)) input = vec3_add(input, cam_right);
        if (IsKeyDown(KEY_A)) input = vec3_sub(input, cam_right);

        if (vec3_len_sq(input) > 0) {
            input = vec3_norm(input);
            player_vel = vec3_add(player_vel, vec3_scale(input, player_speed * dt));
        }

        // Friction
        player_vel = vec3_scale(player_vel, 1.0f - friction * dt);

        // Update position
        player_pos = vec3_add(player_pos, vec3_scale(player_vel, dt));
        player_pos.y = 0.5f; // Keep on ground

        /* ── Orbiting Satellite ───────────────────────────────── */
        orbit_angle += 1.5f * dt;
        Vec3 orbit_offset = vec3(
            cosf(orbit_angle) * orbit_radius,
            orbit_height + sinf(orbit_angle * 2.0f) * 0.5f,
            sinf(orbit_angle) * orbit_radius
        );
        Vec3 satellite_pos = vec3_add(player_pos, orbit_offset);

        /* ── Cube AI: Look at player and hover ────────────────── */
        for (int i = 0; i < MAX_CUBES; i++) {
            // Hover motion
            cubes[i].pos.y += sinf(GetTime() * 2.0f + i) * 0.01f;

            // Rotate to face player
            Vec3 to_player = vec3_sub(player_pos, cubes[i].pos);
            float target_yaw = atan2f(to_player.x, to_player.z);
            cubes[i].rot.y = lerp(cubes[i].rot.y, target_yaw, 3.0f * dt);

            // Spin on local axis
            cubes[i].rot.x += dt * 0.5f;

            // Move toward player if too far
            float dist = vec3_len(to_player);
            if (dist > 8.0f) {
                Vec3 dir = vec3_norm(to_player);
                cubes[i].pos = vec3_add(cubes[i].pos, vec3_scale(dir, 2.0f * dt));
            }
            // Move away if too close
            else if (dist < 5.0f) {
                Vec3 dir = vec3_norm(to_player);
                cubes[i].pos = vec3_sub(cubes[i].pos, vec3_scale(dir, 2.0f * dt));
            }
        }

        /* ── Drawing ──────────────────────────────────────────── */
        BeginDrawing();
        ClearBackground((Color){30, 30, 40, 255});

        Camera3D camera = {
            .position = TO_RL(cam_pos),
            .target = TO_RL(player_pos),
            .up = (Vector3){0, 1, 0},
            .fovy = 60.0f,
            .projection = CAMERA_PERSPECTIVE
        };

        BeginMode3D(camera);

        // Ground grid
        DrawGrid(40, 1.0f);

        // Player sphere
        DrawSphere(TO_RL(player_pos), 0.5f, BLUE);

        // Velocity indicator
        Vec3 vel_end = vec3_add(player_pos, vec3_scale(player_vel, 0.5f));
        DrawLine3D(TO_RL(player_pos), TO_RL(vel_end), YELLOW);

        // Satellite
        DrawSphere(TO_RL(satellite_pos), 0.3f, ORANGE);
        // Line to player
        DrawLine3D(TO_RL(player_pos), TO_RL(satellite_pos), ORANGE);

        // Cubes
        for (int i = 0; i < MAX_CUBES; i++) {
            // Manual transform using our Mat4
            Mat4 transform = mat4_identity();
            transform = mat4_mul(mat4_translate(cubes[i].pos), transform);
            transform = mat4_mul(transform, mat4_rotate_y(cubes[i].rot.y));
            transform = mat4_mul(transform, mat4_rotate_x(cubes[i].rot.x));

            // Draw at transformed position
            DrawCube(TO_RL(cubes[i].pos), 1.0f, 1.0f, 1.0f, cubes[i].color);
            DrawCubeWires(TO_RL(cubes[i].pos), 1.0f, 1.0f, 1.0f, BLACK);

            // Direction arrow (forward vector)
            Vec3 fwd = vec3(sinf(cubes[i].rot.y), 0, cosf(cubes[i].rot.y));
            Vec3 arrow = vec3_add(cubes[i].pos, vec3_scale(fwd, 1.5f));
            DrawLine3D(TO_RL(cubes[i].pos), TO_RL(arrow), BLACK);
        }

        // Axes at origin
        DrawLine3D((Vector3){0,0,0}, (Vector3){3,0,0}, RED);
        DrawLine3D((Vector3){0,0,0}, (Vector3){0,3,0}, GREEN);
        DrawLine3D((Vector3){0,0,0}, (Vector3){0,0,3}, BLUE);

        EndMode3D();

        // HUD
        DrawText("WASD - Move | Mouse - Look | Scroll - Zoom", 10, 10, 18, WHITE);
        DrawText(TextFormat("Position: %.1f, %.1f, %.1f", 
            player_pos.x, player_pos.y, player_pos.z), 10, 35, 16, LIGHTGRAY);
        DrawText(TextFormat("Speed: %.2f", vec3_len(player_vel)), 10, 55, 16, LIGHTGRAY);
        DrawText(TextFormat("Camera: yaw=%.1f pitch=%.1f", 
            rad2deg(cam_yaw), rad2deg(cam_pitch)), 10, 75, 16, LIGHTGRAY);

        DrawFPS(900, 10);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
