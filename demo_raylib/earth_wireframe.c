// earth_pure_linalg.c
#include "raylib.h"
#include "../lib/linalg.h"        // <-- Your header!

#define RADIUS        2.0f
#define SEGMENTS_LAT  36
#define SEGMENTS_LON  72

static inline float clampf(float v, float lo, float hi) {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

int main(void)
{
    InitWindow(1200, 800, "Earth Wireframe - Mouse Orbit + Zoom");
    SetTargetFPS(60);

    // --- Camera (fixed, we'll move the Earth instead) ---
    Camera3D camera = { 0 };
    camera.position   = (Vector3){0.0f, 0.0f, 10.0f};
    camera.target     = (Vector3){0.0f, 0.0f, 0.0f};
    camera.up         = (Vector3){0.0f, 1.0f, 0.0f};
    camera.fovy       = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    // --- Earth rotation (controlled by mouse) ---
    Quat earth_rot = quat_identity();

    // --- Zoom (distance from camera) ---
    float cam_distance = 10.0f;           // Start zoomed out
    const float MIN_DIST = 3.0f;
    const float MAX_DIST = 30.0f;

    // For smooth mouse dragging
    Vector2 last_mouse_pos = {0};
    bool is_dragging = false;

    while (!WindowShouldClose())
    {
        // --------------------- MOUSE ORBIT ---------------------
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            last_mouse_pos = GetMousePosition();
            is_dragging = true;
        }
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
        {
            is_dragging = false;
        }

        if (is_dragging)
        {
            Vector2 mouse = GetMousePosition();
            Vector2 delta = {
                mouse.x - last_mouse_pos.x,
                mouse.y - last_mouse_pos.y
            };
            last_mouse_pos = mouse;

            if (delta.x != 0 || delta.y != 0)
            {
                // Rotate around Y (yaw) and X (pitch) axes
                Quat rot_y = quat_angle_axis(-delta.x * 0.005f, (Vec3){0, 1, 0});
                Quat rot_x = quat_angle_axis(-delta.y * 0.005f, (Vec3){1, 0, 0});

                // Apply: first local X, then global Y (feels natural)
                earth_rot = quat_mul(rot_y, quat_mul(earth_rot, rot_x));
            }
        }

        // --------------------- ZOOM WITH MOUSE WHEEL ---------------------
        float wheel = GetMouseWheelMove();
        if (wheel != 0)
        {
            cam_distance -= wheel * 1.5f;
            cam_distance = clampf(cam_distance, MIN_DIST, MAX_DIST);
        }

        // Update camera position based on zoom
        camera.position.z = cam_distance;

        // --------------------- RENDER ---------------------
        Mat4 model = quat_to_mat4(earth_rot);

        BeginDrawing();
        ClearBackground(BLACK);

        BeginMode3D(camera);

            // === LONGITUDE LINES ===
            for (int i = 0; i < SEGMENTS_LON; i++)
            {
                float a = 2*PI * i / SEGMENTS_LON;
                Color c = (i % (SEGMENTS_LON/12) == 0) ? YELLOW : DARKGRAY;

                Vector3 prev = {0};
                bool first = true;

                for (int j = 0; j <= SEGMENTS_LAT; j++)
                {
                    float phi = PI * j / SEGMENTS_LAT;
                    Vec3 local = {
                        RADIUS * sinf(phi) * cosf(a),
                        RADIUS * cosf(phi),
                        RADIUS * sinf(phi) * sinf(a)
                    };
                    Vec3 w = mat4_mul_vec3(model, local);
                    Vector3 pos = {w.x, w.y, w.z};

                    if (!first) DrawLine3D(prev, pos, c);
                    prev = pos;
                    first = false;
                }
            }

            // === LATITUDE LINES ===
            for (int j = 1; j < SEGMENTS_LAT; j++)
            {
                float phi = PI * j / SEGMENTS_LAT;
                float y = RADIUS * cosf(phi);
                float r = RADIUS * sinf(phi);
                Color c = (j == SEGMENTS_LAT/2) ? GREEN :
                          (j == SEGMENTS_LAT/4 || j == 3*SEGMENTS_LAT/4) ? ORANGE : SKYBLUE;

                Vector3 prev = {0};
                bool first = true;

                for (int i = 0; i <= SEGMENTS_LON; i++)
                {
                    float theta = 2*PI * i / SEGMENTS_LON;
                    Vec3 local = { r * cosf(theta), y, r * sinf(theta) };
                    Vec3 w = mat4_mul_vec3(model, local);
                    Vector3 pos = {w.x, w.y, w.z};

                    if (!first) DrawLine3D(prev, pos, c);
                    prev = pos;
                    first = false;
                }
            }

            // === EQUATOR (RED) ===
            {
                Vector3 prev = {0};
                bool first = true;
                for (int i = 0; i <= 128; i++)
                {
                    float a = 2*PI * i / 128.0f;
                    Vec3 local = { RADIUS * cosf(a), 0.0f, RADIUS * sinf(a) };
                    Vec3 w = mat4_mul_vec3(model, local);
                    Vector3 pos = {w.x, w.y, w.z};
                    if (!first) DrawLine3D(prev, pos, RED);
                    prev = pos;
                    first = false;
                }
            }

        EndMode3D();

        // 2D overlay
        DrawText("Left Mouse Drag = Rotate Earth", 10, 10, 20, RAYWHITE);
        DrawText("Mouse Wheel = Zoom In/Out", 10, 40, 20, RAYWHITE);
        DrawText(TextFormat("Zoom: %.1f", cam_distance), 10, 70, 20, LIME);
        DrawFPS(10, 100);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
