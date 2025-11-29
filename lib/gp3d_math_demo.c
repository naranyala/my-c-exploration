// raylib_gp3d_demo.c
#include "raylib.h"
#include "gp3d_math.h"

typedef struct {
    gp3d_vec3 position;
    float yaw;      // Horizontal rotation (radians)
    float pitch;    // Vertical rotation (radians)
} FreeCamera;

FreeCamera InitFreeCamera(void) {
    FreeCamera cam = {0};
    // Start further back and slightly higher for better view
    cam.position = gp3d_vec3_make(0.0f, 1.5f, 5.0f);
    // Start looking slightly downward to see the cube better
    cam.yaw = GP3D_PI;        // Look towards negative Z (where cube is)
    cam.pitch = -0.2f;        // Look slightly downward
    return cam;
}

// Get forward vector (normalized, no vertical component for movement)
gp3d_vec3 GetCameraForward2D(FreeCamera* cam) {
    gp3d_vec3 forward = gp3d_vec3_make(
        cosf(cam->yaw),
        0,
        sinf(cam->yaw)
    );
    return gp3d_vec3_normalize(forward);
}

// Get forward vector on XZ plane (for movement)
gp3d_vec3 FreeCameraGetForward(FreeCamera* cam) {
    gp3d_vec3 forward = gp3d_vec3_make(
        cosf(cam->yaw),
        0,
        sinf(cam->yaw)
    );
    return gp3d_vec3_normalize(forward);
}

// Get right vector (perpendicular to forward on XZ plane)
gp3d_vec3 FreeCameraGetRight(FreeCamera* cam) {
    gp3d_vec3 forward = FreeCameraGetForward(cam);
    // Right = (-forward.z, 0, forward.x)
    gp3d_vec3 right = gp3d_vec3_make(-forward.z, 0, forward.x);
    return right;
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

static Vector3 ToVector3(gp3d_vec3 v) {
    return (Vector3){v.x, v.y, v.z};
}

int main(void) {
    const int screenWidth = 1280;
    const int screenHeight = 720;
    InitWindow(screenWidth, screenHeight, "gp3d_math.h - Rotating Cube (Natural Mouse Controls)");
    SetTargetFPS(60);

    // 👇 CRITICAL: Enable mouse capture for smooth look
    DisableCursor(); // Hides cursor & enables unlimited mouse movement

    FreeCamera cam = InitFreeCamera();
    float rotation = 0.0f;

    // Cube geometry
    gp3d_vec3 cubeVerts[8] = {
        gp3d_vec3_make(-0.5f, -0.5f, -0.5f),
        gp3d_vec3_make( 0.5f, -0.5f, -0.5f),
        gp3d_vec3_make( 0.5f,  0.5f, -0.5f),
        gp3d_vec3_make(-0.5f,  0.5f, -0.5f),
        gp3d_vec3_make(-0.5f, -0.5f,  0.5f),
        gp3d_vec3_make( 0.5f, -0.5f,  0.5f),
        gp3d_vec3_make( 0.5f,  0.5f,  0.5f),
        gp3d_vec3_make(-0.5f,  0.5f,  0.5f)
    };

    int edges[12][2] = {
        {0,1},{1,2},{2,3},{3,0},
        {4,5},{5,6},{6,7},{7,4},
        {0,4},{1,5},{2,6},{3,7}
    };

    // Mouse control variables
    bool firstMouse = true;
    Vector2 lastMousePos = {0};
    float mouseSensitivity = 0.002f; // More natural sensitivity

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        rotation += dt * 1.2f; // Faster rotation for visibility

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
        // Moving mouse RIGHT should increase yaw (look RIGHT)
        // Moving mouse DOWN should increase pitch (look DOWN)
        cam.yaw   += delta.x * mouseSensitivity;   // Natural horizontal look
        cam.pitch -= delta.y * mouseSensitivity;   // Natural vertical look (note the minus sign)

        // Clamp pitch to avoid flipping (natural limits)
        const float maxPitch = 1.55334f; // ~89 degrees in radians
        if (cam.pitch > maxPitch) cam.pitch = maxPitch;
        if (cam.pitch < -maxPitch) cam.pitch = -maxPitch;

        // Wrap yaw to avoid floating point precision issues
        if (cam.yaw > GP3D_PI * 2) cam.yaw -= GP3D_PI * 2;
        if (cam.pitch < -GP3D_PI * 2) cam.yaw += GP3D_PI * 2;

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

        // Sprint with Shift (feels more natural with higher multiplier)
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

        // === Rotate cube using gp3d_math ===
        gp3d_quat rotQuat = gp3d_quat_from_axis_angle(rotation, 0, 1, 0);
        gp3d_mat4 rotMat = gp3d_quat_to_mat4(rotQuat);
        gp3d_mat4 scaleMat = gp3d_mat4_scale(1.8f, 1.8f, 1.8f);
        gp3d_mat4 model = gp3d_mat4_mul(rotMat, scaleMat);

        Vector3 transformed[8];
        for (int i = 0; i < 8; i++) {
            gp3d_vec3 v = gp3d_mat4_transform_vec3(model, cubeVerts[i], 1.0f);
            transformed[i] = ToVector3(v);
        }

        Camera raylibCam = BuildRaylibCamera(&cam);

        BeginDrawing();
        ClearBackground(RAYWHITE);

        BeginMode3D(raylibCam);
            DrawPlane((Vector3){0, -1, 0}, (Vector2){20, 20}, LIGHTGRAY);
            for (int i = 0; i < 12; i++) {
                Color c = (i < 4) ? RED : (i < 8) ? LIME : BLUE;
                DrawLine3D(transformed[edges[i][0]], transformed[edges[i][1]], c);
            }
            DrawCube((Vector3){0, -1, 0}, 0.1f, 0.1f, 0.1f, BLACK); // Origin marker
        EndMode3D();

        DrawText("Move mouse to look around (Natural FPS controls)", 10, 10, 20, DARKGRAY);
        DrawText("WASD: Move (relative to view)", 10, 40, 20, DARKGRAY);
        DrawText("SPACE / LEFT CTRL: Up / Down", 10, 70, 20, DARKGRAY);
        DrawText("LEFT SHIFT: Sprint (4x speed)", 10, 100, 20, DARKGRAY);
        DrawText("R: Reset camera view", 10, 130, 20, DARKGRAY);
        DrawText("ESC: Toggle mouse cursor", 10, 160, 20, DARKGRAY);
        DrawText("Rotating cube powered by gp3d_math.h", 10, 190, 20, MAROON);
        DrawFPS(10, 220);

        EndDrawing();
    }

    // Re-enable cursor before exit
    EnableCursor();
    CloseWindow();
    return 0;
}
