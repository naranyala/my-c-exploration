// gp3d_collision_demo.c
#include "raylib.h"
#define GP3D_EPSILON 1e-5f
#include "gp3d_math.h"
#include "gp3d_collision.h"

// Custom camera controller
typedef struct {
    gp3d_vec3 position;
    float yaw;
    float pitch;
} FreeCamera;

FreeCamera InitFreeCamera(void) {
    FreeCamera cam = {0};
    cam.position = gp3d_vec3_make(0.0f, 1.5f, 5.0f);
    cam.yaw = GP3D_PI;        // Look towards negative Z (where cube is)
    cam.pitch = -0.2f;        // Look slightly downward
    return cam;
}

// Add this near ToVector3
static gp3d_vec3 FromVector3(Vector3 v) {
    return gp3d_vec3_make(v.x, v.y, v.z);
}

gp3d_vec3 FreeCameraGetForward(FreeCamera* cam) {
    gp3d_vec3 forward = gp3d_vec3_make(cosf(cam->yaw), 0, sinf(cam->yaw));
    return gp3d_vec3_normalize(forward);
}

gp3d_vec3 FreeCameraGetRight(FreeCamera* cam) {
    gp3d_vec3 forward = FreeCameraGetForward(cam);
    return gp3d_vec3_make(-forward.z, 0, forward.x);
}

// Get the actual forward direction for camera view (with pitch)
gp3d_vec3 FreeCameraGetViewForward(FreeCamera* cam) {
    float cosPitch = cosf(cam->pitch);
    gp3d_vec3 forward = gp3d_vec3_make(
        cosf(cam->yaw) * cosPitch,
        sinf(cam->pitch),
        sinf(cam->yaw) * cosPitch
    );
    return gp3d_vec3_normalize(forward);
}

Camera BuildRaylibCamera(FreeCamera* cam) {
    gp3d_vec3 forward = FreeCameraGetViewForward(cam);
    gp3d_vec3 target = gp3d_vec3_add(cam->position, forward);
    gp3d_vec3 up = gp3d_vec3_make(0, 1, 0);

    Camera raylibCam = {0};
    raylibCam.position = (Vector3){cam->position.x, cam->position.y, cam->position.z};
    raylibCam.target   = (Vector3){target.x, target.y, target.z};
    raylibCam.up       = (Vector3){up.x, up.y, up.z};
    raylibCam.fovy     = 45.0f;
    raylibCam.projection = CAMERA_PERSPECTIVE;
    return raylibCam;
}

Vector3 ToVector3(gp3d_vec3 v) {
    return (Vector3){v.x, v.y, v.z};
}

int main(void) {
    const int screenWidth = 1280;
    const int screenHeight = 720;
    InitWindow(screenWidth, screenHeight, "gp3d_collision.h Demo (Natural Mouse Controls)");
    SetTargetFPS(60);
    DisableCursor();

    FreeCamera cam = InitFreeCamera();
    float rotation = 0.0f;

    // Mouse control variables
    bool firstMouse = true;
    Vector2 lastMousePos = {0};
    float mouseSensitivity = 0.002f;

    // Cube geometry (for manual drawing)
    gp3d_vec3 cubeVerts[8] = {
        gp3d_vec3_make(-1.0f, -1.0f, -1.0f),
        gp3d_vec3_make( 1.0f, -1.0f, -1.0f),
        gp3d_vec3_make( 1.0f,  1.0f, -1.0f),
        gp3d_vec3_make(-1.0f,  1.0f, -1.0f),
        gp3d_vec3_make(-1.0f, -1.0f,  1.0f),
        gp3d_vec3_make( 1.0f, -1.0f,  1.0f),
        gp3d_vec3_make( 1.0f,  1.0f,  1.0f),
        gp3d_vec3_make(-1.0f,  1.0f,  1.0f)
    };

    int edges[12][2] = {
        {0,1},{1,2},{2,3},{3,0},
        {4,5},{5,6},{6,7},{7,4},
        {0,4},{1,5},{2,6},{3,7}
    };

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        rotation += dt * 1.5f;

        // === Natural Mouse Look (ALWAYS ACTIVE) ===
        Vector2 mousePos = GetMousePosition();
        
        if (firstMouse) {
            lastMousePos = mousePos;
            firstMouse = false;
        }
        
        Vector2 delta = {
            mousePos.x - lastMousePos.x,
            mousePos.y - lastMousePos.y
        };
        lastMousePos = mousePos;
        
        // Apply natural mouse look (corrected directions)
        cam.yaw   += delta.x * mouseSensitivity;   // Natural horizontal look
        cam.pitch -= delta.y * mouseSensitivity;   // Natural vertical look (note the minus sign)

        // Clamp pitch to avoid flipping (natural limits)
        const float maxPitch = 1.55334f; // ~89 degrees in radians
        if (cam.pitch > maxPitch) cam.pitch = maxPitch;
        if (cam.pitch < -maxPitch) cam.pitch = -maxPitch;

        // Wrap yaw to avoid floating point precision issues
        if (cam.yaw > GP3D_PI * 2) cam.yaw -= GP3D_PI * 2;
        if (cam.yaw < -GP3D_PI * 2) cam.yaw += GP3D_PI * 2;

        // === Camera Movement (VIEW-RELATIVE) ===
        float baseMoveSpeed = 5.0f;
        float moveSpeed = baseMoveSpeed * dt;
        gp3d_vec3 move = gp3d_vec3_make(0, 0, 0);
        gp3d_vec3 forward = FreeCameraGetForward(&cam);
        gp3d_vec3 right = FreeCameraGetRight(&cam);

        // Movement input
        if (IsKeyDown(KEY_W)) move = gp3d_vec3_add(move, forward);
        if (IsKeyDown(KEY_S)) move = gp3d_vec3_sub(move, forward);
        if (IsKeyDown(KEY_D)) move = gp3d_vec3_add(move, right);
        if (IsKeyDown(KEY_A)) move = gp3d_vec3_sub(move, right);
        if (IsKeyDown(KEY_SPACE)) cam.position.y += moveSpeed;
        if (IsKeyDown(KEY_LEFT_CONTROL)) cam.position.y -= moveSpeed;

        // Sprint with Shift
        if (IsKeyDown(KEY_LEFT_SHIFT)) {
            moveSpeed *= 4.0f;
        }

        // Apply movement with proper speed
        if (gp3d_vec3_length(move) > 0.01f) {
            move = gp3d_vec3_normalize(move);
            move = gp3d_vec3_scale(move, moveSpeed);
            cam.position = gp3d_vec3_add(cam.position, move);
        }

        // Reset camera view with R key
        if (IsKeyPressed(KEY_R)) {
            cam = InitFreeCamera();
            firstMouse = true; // Reset mouse tracking
        }

        // Toggle cursor with ESC key
        if (IsKeyPressed(KEY_ESCAPE)) {
            if (IsCursorHidden()) {
                EnableCursor();
            } else {
                DisableCursor();
                firstMouse = true; // Reset mouse tracking
            }
        }

        // === Build cube transform using gp3d_math ===
        gp3d_quat rotQuat = gp3d_quat_from_axis_angle(rotation, 0, 1, 0);
        gp3d_mat4 rotMat = gp3d_quat_to_mat4(rotQuat);
        gp3d_mat4 scaleMat = gp3d_mat4_scale(1.0f, 1.0f, 1.0f);
        gp3d_mat4 model = gp3d_mat4_mul(rotMat, scaleMat);

        // Transform vertices for rendering
        Vector3 transformed[8];
        for (int i = 0; i < 8; i++) {
            gp3d_vec3 v = gp3d_mat4_transform_vec3(model, cubeVerts[i], 1.0f);
            transformed[i] = ToVector3(v);
        }

        // === Collision Setup ===
        // Player sphere (use camera position)
        gp3d_sphere playerSphere = { cam.position, 0.7f };

        gp3d_vec3 aabbMin = FromVector3(transformed[0]);
        gp3d_vec3 aabbMax = FromVector3(transformed[0]);
        for (int i = 1; i < 8; i++) {
            gp3d_vec3 v = FromVector3(transformed[i]);
            aabbMin.x = fminf(aabbMin.x, v.x);
            aabbMin.y = fminf(aabbMin.y, v.y);
            aabbMin.z = fminf(aabbMin.z, v.z);
            aabbMax.x = fmaxf(aabbMax.x, v.x);
            aabbMax.y = fmaxf(aabbMax.y, v.y);
            aabbMax.z = fmaxf(aabbMax.z, v.z);
        }

        gp3d_aabb cubeAABB = {aabbMin, aabbMax};

        // Check collision
        int colliding = gp3d_sphere_vs_aabb(playerSphere, cubeAABB);

        Camera raylibCam = BuildRaylibCamera(&cam);

        BeginDrawing();
        ClearBackground(colliding ? (Color){255, 200, 200, 255} : RAYWHITE); // Subtle red tint on collision

        BeginMode3D(raylibCam);
            DrawPlane((Vector3){0, -2, 0}, (Vector2){20, 20}, LIGHTGRAY);

            // Draw cube edges
            for (int i = 0; i < 12; i++) {
                Color c = colliding ? YELLOW : ((i < 4) ? RED : (i < 8) ? GREEN : BLUE);
                DrawLine3D(transformed[edges[i][0]], transformed[edges[i][1]], c);
            }

            // Draw player sphere (approximate)
            DrawSphereWires(ToVector3(cam.position), 0.7f, 12, 12, colliding ? RED : PURPLE);
        EndMode3D();

        DrawText("Move mouse to look around freely", 10, 10, 20, DARKGRAY);
        DrawText("WASD: Move (relative to view)", 10, 40, 20, DARKGRAY);
        DrawText("SPACE / LEFT CTRL: Up / Down", 10, 70, 20, DARKGRAY);
        DrawText("LEFT SHIFT: Sprint (4x speed)", 10, 100, 20, DARKGRAY);
        DrawText("R: Reset camera view", 10, 130, 20, DARKGRAY);
        DrawText("ESC: Toggle mouse cursor", 10, 160, 20, DARKGRAY);
        DrawText(colliding ? "COLLISION DETECTED!" : "No collision", 10, 190, 25, 
                 colliding ? RED : GREEN);
        DrawFPS(10, 220);
        EndDrawing();
    }

    EnableCursor();
    CloseWindow();
    return 0;
}
