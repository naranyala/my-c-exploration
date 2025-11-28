// Compile with: gcc wormhole.c -o wormhole -lraylib -lm
// Or: gcc wormhole.c -o wormhole -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

#include "../lib/linalg.h"
#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>

// Convert our Vec3 to Raylib's Vector3
Vector3 to_vec3(Vec3 v) {
    return (Vector3){v.x, v.y, v.z};
}

void draw_wormhole_grid(Vec3 center, float max_radius, int ring_count, int segments, float depth) {
    // Draw horizontal grid lines (rings)
    for (int i = 0; i < ring_count; i++) {
        float t = (float)i / (ring_count - 1);
        float z = -depth * t;
        
        // Create funnel shape - radius decreases toward center
        float radius = max_radius * (1.0f - t * 0.95f);
        
        if (radius < 0.1f) radius = 0.1f;
        
        // Draw ring
        for (int j = 0; j < segments; j++) {
            float angle1 = (float)j / segments * 2.0f * PI;
            float angle2 = (float)(j + 1) / segments * 2.0f * PI;
            
            Vec3 p1 = vec3(
                center.x + cosf(angle1) * radius,
                center.y,
                center.z + z + sinf(angle1) * radius
            );
            
            Vec3 p2 = vec3(
                center.x + cosf(angle2) * radius,
                center.y,
                center.z + z + sinf(angle2) * radius
            );
            
            DrawLine3D(to_vec3(p1), to_vec3(p2), WHITE);
        }
    }
    
    // Draw vertical grid lines (meridians)
    int meridian_count = 32;
    for (int j = 0; j < meridian_count; j++) {
        float angle = (float)j / meridian_count * 2.0f * PI;
        
        for (int i = 0; i < ring_count - 1; i++) {
            float t1 = (float)i / (ring_count - 1);
            float t2 = (float)(i + 1) / (ring_count - 1);
            
            float z1 = -depth * t1;
            float z2 = -depth * t2;
            
            float r1 = max_radius * (1.0f - t1 * 0.95f);
            float r2 = max_radius * (1.0f - t2 * 0.95f);
            
            if (r1 < 0.1f) r1 = 0.1f;
            if (r2 < 0.1f) r2 = 0.1f;
            
            Vec3 p1 = vec3(
                center.x + cosf(angle) * r1,
                center.y,
                center.z + z1 + sinf(angle) * r1
            );
            
            Vec3 p2 = vec3(
                center.x + cosf(angle) * r2,
                center.y,
                center.z + z2 + sinf(angle) * r2
            );
            
            DrawLine3D(to_vec3(p1), to_vec3(p2), WHITE);
        }
    }
    
    // Draw the flat grid surface extending outward
    int grid_size = 20;
    float grid_spacing = 2.0f;
    
    // Draw grid in front of wormhole
    for (int x = -grid_size; x <= grid_size; x++) {
        for (int z = -grid_size; z <= grid_size; z++) {
            float world_x = center.x + x * grid_spacing;
            float world_z = center.z + z * grid_spacing;
            
            // Skip if inside the wormhole opening
            float dist = sqrtf(world_x * world_x + world_z * world_z);
            if (dist < max_radius) continue;
            
            // Draw horizontal line
            if (x < grid_size) {
                Vec3 p1 = vec3(world_x, center.y, world_z);
                Vec3 p2 = vec3(world_x + grid_spacing, center.y, world_z);
                DrawLine3D(to_vec3(p1), to_vec3(p2), DARKGRAY);
            }
            
            // Draw vertical line
            if (z < grid_size) {
                Vec3 p1 = vec3(world_x, center.y, world_z);
                Vec3 p2 = vec3(world_x, center.y, world_z + grid_spacing);
                DrawLine3D(to_vec3(p1), to_vec3(p2), DARKGRAY);
            }
        }
    }
}

int main(void) {
    const int screenWidth = 1400;
    const int screenHeight = 900;
    
    InitWindow(screenWidth, screenHeight, "Wormhole Grid - Custom Linear Algebra");
    
    Camera3D camera = {0};
    camera.position = (Vector3){0.0f, 25.0f, 35.0f};
    camera.target = (Vector3){0.0f, 0.0f, -15.0f};
    camera.up = (Vector3){0.0f, 1.0f, 0.0f};
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;
    
    Vec3 wormhole_center = vec3(0.0f, 0.0f, 0.0f);
    float wormhole_radius = 15.0f;
    float wormhole_depth = 60.0f;
    int ring_count = 80;
    int segments = 64;
    
    bool enable_camera_control = false;
    
    SetTargetFPS(60);
    
    while (!WindowShouldClose()) {
        // Camera control
        if (IsKeyPressed(KEY_C)) enable_camera_control = !enable_camera_control;
        
        if (enable_camera_control) {
            UpdateCamera(&camera, CAMERA_FREE);
        }
        
        // Drawing
        BeginDrawing();
        ClearBackground(BLACK);
        
        BeginMode3D(camera);
        
        // Draw the wormhole grid
        draw_wormhole_grid(wormhole_center, wormhole_radius, ring_count, segments, wormhole_depth);
        
        EndMode3D();
        
        // UI
        DrawFPS(10, 10);
        DrawText("WORMHOLE GRID", 10, 40, 30, WHITE);
        DrawText("Using custom linear algebra library", 10, 75, 16, LIGHTGRAY);
        DrawText(TextFormat("Rings: %d | Segments: %d", ring_count, segments), 10, 100, 16, GRAY);
        DrawText(enable_camera_control ? "C: Camera Control ON (WASD + Mouse)" : "C: Enable Camera Control", 
                 10, screenHeight - 30, 16, GRAY);
        
        EndDrawing();
    }
    
    CloseWindow();
    return 0;
}
