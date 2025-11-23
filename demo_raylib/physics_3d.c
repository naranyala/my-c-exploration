/*
 * TinyPhys3D + Raylib Demo
 * 
 * Compile with:
 *   gcc tinyphys_raylib_demo.c -o demo -lraylib -lm
 * 
 * Or on Windows with MinGW:
 *   gcc tinyphys_raylib_demo.c -o demo.exe -lraylib -lopengl32 -lgdi32 -lwinmm
 * 
 * Controls:
 *   - Mouse: Rotate camera
 *   - SPACE: Spawn a random sphere
 *   - R: Reset simulation
 *   - ESC: Exit
 */

#define TINYPHYS_IMPLEMENTATION
#include "../lib/tinyphys_3d.h"

#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_BODIES 100
#define SPAWN_HEIGHT 15.0f

/* Helper function to convert tp3d_vec3 to Vector3 */
Vector3 to_vector3(tp3d_vec3 v) {
    return (Vector3){v.x, v.y, v.z};
}

/* Helper function to convert Vector3 to tp3d_vec3 */
tp3d_vec3 to_tp3d_vec3(Vector3 v) {
    return (tp3d_vec3){v.x, v.y, v.z};
}

/* Generate random color */
Color random_color(void) {
    return (Color){
        GetRandomValue(100, 255),
        GetRandomValue(100, 255),
        GetRandomValue(100, 255),
        255
    };
}

/* Store color for each body */
typedef struct {
    Color color;
} BodyVisual;

BodyVisual body_visuals[MAX_BODIES];

/* Initialize physics world */
void init_world(tp3d_world* world) {
    world->body_count = 0;
    
    /* Create ground plane (large static sphere) */
    tp3d_body* ground = tp3d_add_body(world);
    tp3d_body_set_sphere(ground, 50.0f);
    ground->position = (tp3d_vec3){0, -50.0f, 0};
    ground->is_static = 1;
    ground->restitution = 0.3f;
    body_visuals[0].color = DARKGRAY;
    
    /* Create some walls (static spheres) */
    for (int i = 0; i < 4; i++) {
        tp3d_body* wall = tp3d_add_body(world);
        tp3d_body_set_sphere(wall, 3.0f);
        float angle = i * PI / 2.0f;
        wall->position = (tp3d_vec3){
            cosf(angle) * 10.0f,
            3.0f,
            sinf(angle) * 10.0f
        };
        wall->is_static = 1;
        wall->restitution = 0.8f;
        body_visuals[world->body_count - 1].color = GRAY;
    }
    
    /* Spawn some initial spheres */
    for (int i = 0; i < 5; i++) {
        tp3d_body* ball = tp3d_add_body(world);
        float radius = 0.5f + (float)GetRandomValue(0, 10) / 20.0f;
        tp3d_body_set_sphere(ball, radius);
        ball->position = (tp3d_vec3){
            (float)GetRandomValue(-5, 5),
            SPAWN_HEIGHT + i * 2.0f,
            (float)GetRandomValue(-5, 5)
        };
        ball->mass = radius * radius * radius;
        ball->inv_mass = 1.0f / ball->mass;
        ball->restitution = 0.6f;
        ball->friction = 0.4f;
        body_visuals[world->body_count - 1].color = random_color();
    }
}

/* Spawn a new sphere */
void spawn_sphere(tp3d_world* world) {
    if (world->body_count >= MAX_BODIES) return;
    
    tp3d_body* ball = tp3d_add_body(world);
    float radius = 0.5f + (float)GetRandomValue(0, 10) / 20.0f;
    tp3d_body_set_sphere(ball, radius);
    ball->position = (tp3d_vec3){
        (float)GetRandomValue(-8, 8),
        SPAWN_HEIGHT,
        (float)GetRandomValue(-8, 8)
    };
    ball->mass = radius * radius * radius;
    ball->inv_mass = 1.0f / ball->mass;
    ball->restitution = 0.5f + (float)GetRandomValue(0, 40) / 100.0f;
    ball->friction = 0.2f + (float)GetRandomValue(0, 40) / 100.0f;
    
    /* Give it a random initial velocity */
    ball->velocity = (tp3d_vec3){
        (float)GetRandomValue(-3, 3),
        0,
        (float)GetRandomValue(-3, 3)
    };
    
    body_visuals[world->body_count - 1].color = random_color();
}

/* Draw physics bodies */
void draw_bodies(tp3d_world* world) {
    for (int i = 0; i < world->body_count; i++) {
        tp3d_body* body = &world->bodies[i];
        Vector3 pos = to_vector3(body->position);
        
        if (body->shape.type == TP3D_SHAPE_SPHERE) {
            DrawSphere(pos, body->shape.radius, body_visuals[i].color);
            DrawSphereWires(pos, body->shape.radius, 8, 8, BLACK);
        }
    }
}

/* Draw UI */
void draw_ui(tp3d_world* world, int fps) {
    DrawText("TinyPhys3D + Raylib Demo", 10, 10, 20, RAYWHITE);
    DrawText(TextFormat("FPS: %d", fps), 10, 40, 20, LIME);
    DrawText(TextFormat("Bodies: %d/%d", world->body_count, MAX_BODIES), 10, 70, 20, SKYBLUE);
    
    DrawText("Controls:", 10, 110, 16, YELLOW);
    DrawText("SPACE - Spawn sphere", 10, 130, 14, WHITE);
    DrawText("R - Reset simulation", 10, 150, 14, WHITE);
    DrawText("Mouse - Rotate camera", 10, 170, 14, WHITE);
    DrawText("ESC - Exit", 10, 190, 14, WHITE);
    
    /* Draw velocity info for first few dynamic bodies */
    int y_offset = 230;
    DrawText("Dynamic Bodies Info:", 10, y_offset, 14, ORANGE);
    y_offset += 20;
    
    int count = 0;
    for (int i = 0; i < world->body_count && count < 3; i++) {
        tp3d_body* body = &world->bodies[i];
        if (!body->is_static) {
            float speed = tp3d_vec3_length(body->velocity);
            DrawText(TextFormat("Body %d: v=%.2f m/s", i, speed), 10, y_offset, 12, LIGHTGRAY);
            y_offset += 15;
            count++;
        }
    }
}

/* Draw grid */
void draw_grid(void) {
    int slices = 20;
    float spacing = 2.0f;
    
    for (int i = -slices; i <= slices; i++) {
        DrawLine3D(
            (Vector3){i * spacing, 0, -slices * spacing},
            (Vector3){i * spacing, 0, slices * spacing},
            (i == 0) ? BLUE : DARKGRAY
        );
        DrawLine3D(
            (Vector3){-slices * spacing, 0, i * spacing},
            (Vector3){slices * spacing, 0, i * spacing},
            (i == 0) ? RED : DARKGRAY
        );
    }
}

int main(void) {
    /* Initialize window */
    const int screenWidth = 1200;
    const int screenHeight = 600;
    
    InitWindow(screenWidth, screenHeight, "TinyPhys3D + Raylib Demo");
    SetTargetFPS(60);
    
    /* Initialize random seed */
    srand(time(NULL));
    
    /* Setup camera */
    Camera3D camera = {0};
    camera.position = (Vector3){20.0f, 15.0f, 20.0f};
    camera.target = (Vector3){0.0f, 5.0f, 0.0f};
    camera.up = (Vector3){0.0f, 1.0f, 0.0f};
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;
    
    /* Create physics world */
    tp3d_world* world = tp3d_create_world(MAX_BODIES);
    init_world(world);
    
    /* Physics accumulator for fixed timestep */
    float accumulator = 0.0f;
    float fixed_dt = 1.0f / 60.0f;
    
    /* Main game loop */
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        accumulator += dt;
        
        /* Handle input */
        if (IsKeyPressed(KEY_SPACE)) {
            spawn_sphere(world);
        }
        
        if (IsKeyPressed(KEY_R)) {
            init_world(world);
        }
        
        /* Update camera */
        UpdateCamera(&camera, CAMERA_ORBITAL);
        
        /* Fixed timestep physics update */
        while (accumulator >= fixed_dt) {
            tp3d_step(world, fixed_dt);
            accumulator -= fixed_dt;
        }
        
        /* Render */
        BeginDrawing();
        ClearBackground(DARKBLUE);
        
        BeginMode3D(camera);
        
        /* Draw grid */
        draw_grid();
        
        /* Draw physics bodies */
        draw_bodies(world);
        
        /* Draw ground indicator */
        DrawCircle3D((Vector3){0, 0, 0}, 50.0f, (Vector3){1, 0, 0}, 90.0f, Fade(GREEN, 0.3f));
        
        EndMode3D();
        
        /* Draw UI */
        draw_ui(world, GetFPS());
        
        EndDrawing();
    }
    
    /* Cleanup */
    tp3d_destroy_world(world);
    CloseWindow();
    
    return 0;
}
