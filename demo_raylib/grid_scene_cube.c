#include "raylib.h"
#include <math.h>

int main(void) {
  const int screenWidth = 1024;
  const int screenHeight = 768;

  InitWindow(screenWidth, screenHeight, "3D Cube in Grid Scene - C + Raylib");
  SetTargetFPS(60);

  // Define the camera
  Camera3D camera = {0};
  camera.position = (Vector3){10.0f, 10.0f, 10.0f};
  camera.target = (Vector3){0.0f, 0.0f, 0.0f};
  camera.up = (Vector3){0.0f, 1.0f, 0.0f};
  camera.fovy = 45.0f;
  camera.projection = CAMERA_PERSPECTIVE;

  float cubeRotation = 0.0f;
  float cameraAngle = 0.0f;
  const float cameraRadius = 15.0f;

  // Small cube positions
  Vector3 smallCubePositions[4] = {{-5.0f, 0.5f, -5.0f},
                                   {5.0f, 0.5f, -5.0f},
                                   {-5.0f, 0.5f, 5.0f},
                                   {5.0f, 0.5f, 5.0f}};

  Color smallCubeColors[4] = {{100, 200, 255, 255},
                              {255, 200, 100, 255},
                              {100, 255, 150, 255},
                              {255, 100, 255, 255}};

  while (!WindowShouldClose()) {
    // Update
    cubeRotation += 1.0f;

    // Manual camera rotation with arrow keys
    if (IsKeyDown(KEY_RIGHT))
      cameraAngle += 0.02f;
    if (IsKeyDown(KEY_LEFT))
      cameraAngle -= 0.02f;

    // Update camera position using trigonometry
    camera.position.x = cosf(cameraAngle) * cameraRadius;
    camera.position.z = sinf(cameraAngle) * cameraRadius;

    // Draw
    BeginDrawing();
    ClearBackground((Color){20, 20, 30, 255});

    BeginMode3D(camera);

    // Draw grid
    const int gridSize = 20;
    const float gridSpacing = 1.0f;

    for (int i = -gridSize; i <= gridSize; i++) {
      float pos = (float)i * gridSpacing;
      float startZ = (float)(-gridSize) * gridSpacing;
      float endZ = (float)(gridSize)*gridSpacing;
      float startX = (float)(-gridSize) * gridSpacing;
      float endX = (float)(gridSize)*gridSpacing;

      // Lines along Z axis
      DrawLine3D((Vector3){pos, 0.0f, startZ}, (Vector3){pos, 0.0f, endZ},
                 (Color){50, 50, 70, 255});

      // Lines along X axis
      DrawLine3D((Vector3){startX, 0.0f, pos}, (Vector3){endX, 0.0f, pos},
                 (Color){50, 50, 70, 255});
    }

    // Draw axis indicators
    DrawLine3D((Vector3){0.0f, 0.0f, 0.0f}, (Vector3){3.0f, 0.0f, 0.0f}, RED);
    DrawLine3D((Vector3){0.0f, 0.0f, 0.0f}, (Vector3){0.0f, 3.0f, 0.0f}, GREEN);
    DrawLine3D((Vector3){0.0f, 0.0f, 0.0f}, (Vector3){0.0f, 0.0f, 3.0f}, BLUE);

    // Draw rotating cube at center
    DrawCubeV((Vector3){0.0f, 1.5f, 0.0f}, (Vector3){2.0f, 2.0f, 2.0f},
              (Color){255, 100, 100, 255});

    // Draw cube wireframe
    DrawCubeWiresV((Vector3){0.0f, 1.5f, 0.0f}, (Vector3){2.0f, 2.0f, 2.0f},
                   WHITE);

    // Draw some smaller cubes on the grid
    for (int i = 0; i < 4; i++) {
      DrawCubeV(smallCubePositions[i], (Vector3){1.0f, 1.0f, 1.0f},
                smallCubeColors[i]);
      DrawCubeWiresV(smallCubePositions[i], (Vector3){1.0f, 1.0f, 1.0f},
                     DARKGRAY);
    }

    EndMode3D();

    // Draw info text in 2D
    DrawRectangle(10, 10, 320, 100, (Color){0, 0, 0, 150});
    DrawText("3D Cube in Grid Scene", 20, 20, 20, WHITE);
    DrawText("Controls:", 20, 45, 15, LIGHTGRAY);
    DrawText("  LEFT/RIGHT arrows - Rotate camera", 20, 65, 15, LIGHTGRAY);
    DrawText("  ESC - Exit", 20, 85, 15, LIGHTGRAY);

    DrawFPS(screenWidth - 100, 10);

    EndDrawing();
  }

  CloseWindow();
  return 0;
}
