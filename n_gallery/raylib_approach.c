#define TINYFILEDIALOGS_IMPLEMENTATION
#include "tinyfiledialogs.h"
#include "raylib.h"  // Includes built-in GLTF support via cgltf

int main(void) {
    const char *folder = tinyfd_selectFolderDialog("Select media folder", NULL);
    if (!folder) return 0;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT);
    InitWindow(1200, 800, "2D/3D Media Gallery");
    SetTargetFPS(60);

    // Load all files: images + 3D
    FilePathList files = LoadDirectoryFilesEx(folder, 
        "*.png;*.jpg;*.jpeg;*.bmp;*.gltf;*.glb", false);
    
    if (files.count == 0) return 1;  // No files

    int current = 0;
    Camera3D cam = { 0 };  // 3D camera for models
    cam.position = (Vector3){ 4.0f, 2.0f, 4.0f };
    cam.target = (Vector3){ 0.0f, 1.0f, 0.0f };
    cam.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    cam.fovy = 45.0f;
    SetCameraMode(CAMERA_ORBITAL, cam);

    // Current asset: Texture for 2D, Model for 3D
    Texture2D tex = {0};
    Model model = {0};
    bool is3D = false;
    const char *currentPath = files.paths[current];

    // Load initial asset
    if (IsFileExtension(currentPath, ".gltf;.glb")) {
        is3D = true;
        model = LoadModel(currentPath);  // Parses GLTF/GLB fully
        UpdateCamera(&cam);  // Fit to model bounds if needed
    } else {
        tex = LoadTexture(currentPath);
    }

    while (!WindowShouldClose()) {
        UpdateCamera(&cam);  // Orbit controls

        // Navigation
        if (IsKeyPressed(KEY_RIGHT) && current < files.count - 1) {
            current++; LoadNextAsset(&tex, &model, &is3D, files.paths[current]);
        }
        if (IsKeyPressed(KEY_LEFT) && current > 0) {
            current--; LoadNextAsset(&tex, &model, &is3D, files.paths[current]);
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (is3D) {
            BeginMode3D(cam);
            DrawModel(model, (Vector3){0,0,0}, 1.0f, WHITE);
            DrawGrid(10, 1.0f);  // Floor grid for reference
            EndMode3D();
        } else {
            DrawTexturePro(tex, (Rectangle){0,0,(float)tex.width,(float)tex.height},
                           (Rectangle){0,0,(float)GetScreenWidth(),(float)GetScreenHeight()},
                           (Vector2){0,0}, 0.0f, WHITE);
        }

        // UI overlay
        DrawText(TextFormat("%d / %d - %s", current+1, files.count, GetFileName(files.paths[current])), 20, 20, 20, DARKGRAY);
        if (is3D) DrawText("Mouse: Orbit | WASD: Pan | Scroll: Zoom", 20, 50, 16, GRAY);

        EndDrawing();
    }

    // Cleanup
    if (is3D) UnloadModel(model); else UnloadTexture(tex);
    UnloadDirectoryFiles(files);
    CloseWindow();
    return 0;
}

// Helper: Load next asset (detect 2D/3D)
void LoadNextAsset(Texture2D *tex, Model *model, bool *is3D, const char *path) {
    UnloadTexture(*tex); *tex = (Texture2D){0};
    UnloadModel(*model); *model = (Model){0};
    if (IsFileExtension(path, ".gltf;.glb")) {
        *is3D = true;
        *model = LoadModel(path);
    } else {
        *is3D = false;
        *tex = LoadTexture(path);
    }
}
