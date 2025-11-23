// Compile with: gcc demo.c -o demo -lraylib -lm
// Or on some systems: gcc demo.c -o demo -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

#include "../lib/linalg.h"
#include "raylib.h"
#include <stdio.h>


// Convert our Vec3 to Raylib's Vector3
Vector3 to_raylib_vec3(Vec3 v) {
    return (Vector3){v.x, v.y, v.z};
}

// Convert Raylib's Vector3 to our Vec3
Vec3 from_raylib_vec3(Vector3 v) {
    return vec3(v.x, v.y, v.z);
}

// Helper clamp function
float clamp_f(float value, float min, float max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

typedef struct {
    Vec3 position;
    Vec3 velocity;
    float radius;
    Color color;
} Sphere;

void update_sphere(Sphere *s, float dt, Vec3 bounds) {
    // Update position using our linalg
    s->position = vec3_add(s->position, vec3_mul(s->velocity, dt));
    
    // Bounce off walls
    if (s->position.x - s->radius < -bounds.x || s->position.x + s->radius > bounds.x) {
        s->velocity.x *= -1;
        s->position.x = clamp_f(s->position.x, -bounds.x + s->radius, bounds.x - s->radius);
    }
    if (s->position.y - s->radius < 0 || s->position.y + s->radius > bounds.y * 2) {
        s->velocity.y *= -1;
        s->position.y = clamp_f(s->position.y, s->radius, bounds.y * 2 - s->radius);
    }
    if (s->position.z - s->radius < -bounds.z || s->position.z + s->radius > bounds.z) {
        s->velocity.z *= -1;
        s->position.z = clamp_f(s->position.z, -bounds.z + s->radius, bounds.z - s->radius);
    }
}

int main(void) {
    const int screenWidth = 1200;
    const int screenHeight = 800;
    
    InitWindow(screenWidth, screenHeight, "Raylib + Custom Linear Algebra Demo");
    
    // Setup camera
    Camera3D camera = { 0 };
    camera.position = (Vector3){ 15.0f, 15.0f, 15.0f };
    camera.target = (Vector3){ 0.0f, 5.0f, 0.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;
    
    // Create bouncing spheres using our Vec3
    const int SPHERE_COUNT = 8;
    Sphere spheres[SPHERE_COUNT];
    
    for (int i = 0; i < SPHERE_COUNT; i++) {
        spheres[i].position = vec3(
            GetRandomValue(-8, 8),
            GetRandomValue(2, 15),
            GetRandomValue(-8, 8)
        );
        spheres[i].velocity = vec3(
            GetRandomValue(-5, 5),
            GetRandomValue(-5, 5),
            GetRandomValue(-5, 5)
        );
        spheres[i].radius = 0.5f + (float)GetRandomValue(0, 10) / 10.0f;
        spheres[i].color = (Color){
            GetRandomValue(100, 255),
            GetRandomValue(100, 255),
            GetRandomValue(100, 255),
            255
        };
    }
    
    Vec3 bounds = vec3(10.0f, 10.0f, 10.0f);
    float time = 0.0f;
    
    SetTargetFPS(60);
    
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        time += dt;
        
        // Update camera orbit using our linalg
        Vec3 cam_center = vec3(0.0f, 5.0f, 0.0f);
        float cam_radius = 20.0f;
        float cam_angle = time * 0.3f;
        
        Vec3 cam_pos = vec3(
            cam_center.x + cosf(cam_angle) * cam_radius,
            cam_center.y + 5.0f + sinf(time * 0.5f) * 3.0f,
            cam_center.z + sinf(cam_angle) * cam_radius
        );
        
        camera.position = to_raylib_vec3(cam_pos);
        camera.target = to_raylib_vec3(cam_center);
        
        // Update spheres
        for (int i = 0; i < SPHERE_COUNT; i++) {
            update_sphere(&spheres[i], dt, bounds);
            
            // Simple sphere-sphere collision using our linalg functions
            for (int j = i + 1; j < SPHERE_COUNT; j++) {
                Vec3 delta = vec3_sub(spheres[j].position, spheres[i].position);
                float dist = vec3_len(delta);
                float min_dist = spheres[i].radius + spheres[j].radius;
                
                if (dist < min_dist && dist > 0.001f) {
                    // Collision! Use our normalize and reflect operations
                    Vec3 normal = vec3_normalize(delta);
                    
                    // Separate spheres
                    float overlap = min_dist - dist;
                    Vec3 separation = vec3_mul(normal, overlap * 0.5f);
                    spheres[i].position = vec3_sub(spheres[i].position, separation);
                    spheres[j].position = vec3_add(spheres[j].position, separation);
                    
                    // Reflect velocities
                    Vec3 rel_vel = vec3_sub(spheres[j].velocity, spheres[i].velocity);
                    float vel_along_normal = vec3_dot(rel_vel, normal);
                    
                    if (vel_along_normal < 0) {
                        Vec3 impulse = vec3_mul(normal, vel_along_normal);
                        spheres[i].velocity = vec3_add(spheres[i].velocity, impulse);
                        spheres[j].velocity = vec3_sub(spheres[j].velocity, impulse);
                    }
                }
            }
        }
        
        // Drawing
        BeginDrawing();
        ClearBackground(RAYWHITE);
        
        BeginMode3D(camera);
        
        // Draw ground grid
        DrawGrid(20, 2.0f);
        
        // Draw bounding box using our Vec3 corners
        Vec3 corners[8] = {
            vec3(-bounds.x, 0, -bounds.z),
            vec3( bounds.x, 0, -bounds.z),
            vec3( bounds.x, 0,  bounds.z),
            vec3(-bounds.x, 0,  bounds.z),
            vec3(-bounds.x, bounds.y * 2, -bounds.z),
            vec3( bounds.x, bounds.y * 2, -bounds.z),
            vec3( bounds.x, bounds.y * 2,  bounds.z),
            vec3(-bounds.x, bounds.y * 2,  bounds.z),
        };
        
        // Draw edges
        for (int i = 0; i < 4; i++) {
            DrawLine3D(to_raylib_vec3(corners[i]), 
                      to_raylib_vec3(corners[(i + 1) % 4]), GRAY);
            DrawLine3D(to_raylib_vec3(corners[i + 4]), 
                      to_raylib_vec3(corners[((i + 1) % 4) + 4]), GRAY);
            DrawLine3D(to_raylib_vec3(corners[i]), 
                      to_raylib_vec3(corners[i + 4]), GRAY);
        }
        
        // Draw spheres with velocity vectors
        for (int i = 0; i < SPHERE_COUNT; i++) {
            DrawSphere(to_raylib_vec3(spheres[i].position), 
                      spheres[i].radius, 
                      spheres[i].color);
            
            // Draw velocity vector using our linalg
            Vec3 vel_end = vec3_add(spheres[i].position, 
                                    vec3_mul(vec3_normalize(spheres[i].velocity), 2.0f));
            DrawLine3D(to_raylib_vec3(spheres[i].position), 
                      to_raylib_vec3(vel_end), 
                      BLACK);
        }
        
        // Demo: Show vector operations with a rotating cube
        Vec3 cube_pos = vec3(0.0f, 15.0f, 0.0f);
        Vec3 axis_x = vec3(2.0f, 0.0f, 0.0f);
        Vec3 axis_y = vec3(0.0f, 2.0f, 0.0f);
        Vec3 axis_z = vec3(0.0f, 0.0f, 2.0f);
        
        // Rotate axes using simple rotation (demonstrating our math)
        float angle = time;
        float c = cosf(angle);
        float s = sinf(angle);
        
        Vec3 rotated_x = vec3(
            axis_x.x * c - axis_x.z * s,
            axis_x.y,
            axis_x.x * s + axis_x.z * c
        );
        Vec3 rotated_z = vec3(
            axis_z.x * c - axis_z.z * s,
            axis_z.y,
            axis_z.x * s + axis_z.z * c
        );
        
        // Draw coordinate system
        DrawLine3D(to_raylib_vec3(cube_pos), 
                  to_raylib_vec3(vec3_add(cube_pos, rotated_x)), RED);
        DrawLine3D(to_raylib_vec3(cube_pos), 
                  to_raylib_vec3(vec3_add(cube_pos, axis_y)), GREEN);
        DrawLine3D(to_raylib_vec3(cube_pos), 
                  to_raylib_vec3(vec3_add(cube_pos, rotated_z)), BLUE);
        
        EndMode3D();
        
        // UI
        DrawFPS(10, 10);
        DrawText("Custom Linear Algebra Library Demo", 10, 40, 20, DARKGRAY);
        DrawText("Using Vec3 operations: add, sub, mul, normalize, dot, cross, lerp", 10, 70, 16, GRAY);
        
        // Show some calculations using our linalg
        if (SPHERE_COUNT > 1) {
            Vec3 s0 = spheres[0].position;
            Vec3 s1 = spheres[1].position;
            float dist = vec3_dist(s0, s1);
            Vec3 midpoint = vec3_lerp(s0, s1, 0.5f);
            
            DrawText(TextFormat("Sphere 0 to 1 distance: %.2f", dist), 10, 100, 16, DARKBLUE);
            DrawText(TextFormat("Midpoint: (%.1f, %.1f, %.1f)", midpoint.x, midpoint.y, midpoint.z), 
                    10, 120, 16, DARKBLUE);
            
            // Draw line between first two spheres
            DrawLine3D(to_raylib_vec3(s0), to_raylib_vec3(s1), PURPLE);
            DrawSphere(to_raylib_vec3(midpoint), 0.2f, PURPLE);
        }
        
        EndDrawing();
    }
    
    CloseWindow();
    return 0;
}
