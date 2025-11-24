#include "raylib.h"
#include "raymath.h"
#include <math.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

// typedef struct {
//     float x, y, z, w;
// } Vector4;

Vector4 vertices4D[16];
int edges[32][2] = {
    // Inner cube edges
    {0,1},{1,3},{3,2},{2,0},{4,5},{5,7},{7,6},{6,4},{0,4},{1,5},{2,6},{3,7},
    // Outer cube edges
    {8,9},{9,11},{11,10},{10,8},{12,13},{13,15},{15,14},{14,12},{8,12},{9,13},{10,14},{11,15},
    // Connecting edges
    {0,8},{1,9},{2,10},{3,11},{4,12},{5,13},{6,14},{7,15}
};

void InitializeHypercube(float size) {
    int index = 0;
    for (int w = 0; w < 2; w++) {
        for (int z = 0; z < 2; z++) {
            for (int y = 0; y < 2; y++) {
                for (int x = 0; x < 2; x++) {
                    vertices4D[index].x = (x * 2 - 1) * size;
                    vertices4D[index].y = (y * 2 - 1) * size;
                    vertices4D[index].z = (z * 2 - 1) * size;
                    vertices4D[index].w = (w * 2 - 1) * size;
                    index++;
                }
            }
        }
    }
}

Vector4 RotateXW(Vector4 v, float angle) {
    float cosA = cosf(angle);
    float sinA = sinf(angle);
    Vector4 result;
    result.x = v.x * cosA - v.w * sinA;
    result.y = v.y;
    result.z = v.z;
    result.w = v.x * sinA + v.w * cosA;
    return result;
}

Vector4 RotateYW(Vector4 v, float angle) {
    float cosA = cosf(angle);
    float sinA = sinf(angle);
    Vector4 result;
    result.x = v.x;
    result.y = v.y * cosA - v.w * sinA;
    result.z = v.z;
    result.w = v.y * sinA + v.w * cosA;
    return result;
}

Vector4 RotateZW(Vector4 v, float angle) {
    float cosA = cosf(angle);
    float sinA = sinf(angle);
    Vector4 result;
    result.x = v.x;
    result.y = v.y;
    result.z = v.z * cosA - v.w * sinA;
    result.w = v.z * sinA + v.w * cosA;
    return result;
}

Vector3 Project4Dto3D(Vector4 v, float distance) {
    float w = 1.0f / (distance - v.w);
    Vector3 result;
    result.x = v.x * w;
    result.y = v.y * w;
    result.z = v.z * w;
    return result;
}

Vector2 Project3Dto2D(Vector3 v, float distance) {
    float scale = distance / (distance + v.z);
    Vector2 result;
    result.x = v.x * scale * 200 + SCREEN_WIDTH / 2;
    result.y = v.y * scale * 200 + SCREEN_HEIGHT / 2;
    return result;
}

int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Tesseract (4D Hypercube) Demo");
    SetTargetFPS(60);
    
    InitializeHypercube(1.0f);
    
    float angleXW = 0.0f;
    float angleYW = 0.0f;
    float angleZW = 0.0f;
    
    while (!WindowShouldClose()) {
        // Update rotation angles
        angleXW += 0.01f;
        angleYW += 0.008f;
        angleZW += 0.006f;
        
        // Rotate and project vertices
        Vector4 rotated[16];
        Vector3 projected3D[16];
        Vector2 projected2D[16];
        
        for (int i = 0; i < 16; i++) {
            rotated[i] = vertices4D[i];
            rotated[i] = RotateXW(rotated[i], angleXW);
            rotated[i] = RotateYW(rotated[i], angleYW);
            rotated[i] = RotateZW(rotated[i], angleZW);
            
            projected3D[i] = Project4Dto3D(rotated[i], 3.0f);
            projected2D[i] = Project3Dto2D(projected3D[i], 3.0f);
        }
        
        BeginDrawing();
        ClearBackground(BLACK);
        
        // Draw edges
        for (int i = 0; i < 32; i++) {
            int v1 = edges[i][0];
            int v2 = edges[i][1];
            
            // Color edges based on their type
            Color edgeColor;
            if (i < 12) edgeColor = BLUE;        // Inner cube
            else if (i < 24) edgeColor = RED;    // Outer cube
            else edgeColor = GREEN;               // Connecting edges
            
            DrawLineEx(projected2D[v1], projected2D[v2], 1.5f, edgeColor);
        }
        
        // Draw vertices
        for (int i = 0; i < 16; i++) {
            DrawCircleV(projected2D[i], 3.0f, YELLOW);
        }
        
        DrawText("4D Hypercube (Tesseract)", 10, 10, 20, WHITE);
        DrawText("Blue: Inner cube | Red: Outer cube | Green: Connections", 10, 35, 16, GRAY);
        
        EndDrawing();
    }
    
    CloseWindow();
    return 0;
}
