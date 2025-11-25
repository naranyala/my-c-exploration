// main.c - Raylib demo with tinyphys_2d and collision.h
#include "raylib.h"
#include "tinyphys_2d.h"
#include "collision.h"
#include <stdlib.h>
#include <time.h>

#define MAX_BODIES 50
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

// Convert raylib color to integer
int ColorToInt(Color color) {
    return ((color.r & 0xFF) << 24) | 
           ((color.g & 0xFF) << 16) | 
           ((color.b & 0xFF) << 8) | 
           (color.a & 0xFF);
}

// Convert integer to raylib color
Color IntToColor(int value) {
    return (Color){
        (unsigned char)((value >> 24) & 0xFF),
        (unsigned char)((value >> 16) & 0xFF),
        (unsigned char)((value >> 8) & 0xFF),
        (unsigned char)(value & 0xFF)
    };
}

int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Physics Demo - TinyPhys2D + Collision");
    SetTargetFPS(60);
    
    // Initialize physics world
    tp_body bodies[MAX_BODIES];
    tp_world world;
    tp_world_init(&world, bodies, MAX_BODIES, 1.0f/60.0f);
    world.gravity_y = 400.0f; // Moderate gravity
    
    // Store colors for each body
    int bodyColors[MAX_BODIES];
    
    // Create some random bodies
    srand(time(NULL));
    int bodyCount = 15; // Start with fewer bodies
    
    for (int i = 0; i < bodyCount; i++) {
        tp_body* body = tp_world_add_body(&world);
        
        // Random position in top half of screen
        body->x = 50 + (rand() % (SCREEN_WIDTH - 100));
        body->y = 50 + (rand() % 200);
        
        // Random small initial velocity
        body->vx = (rand() % 100) - 50;
        body->vy = (rand() % 50) - 25;
        
        // Random mass
        float mass = 1.0f + (rand() % 10);
        tp_set_mass(body, mass);
        
        // Random shape
        if (rand() % 2 == 0) {
            // Circle
            body->shape = TP_SHAPE_CIRCLE;
            body->radius = 15 + (rand() % 25);
            body->restitution = 0.3f + ((float)rand() / RAND_MAX) * 0.4f;
        } else {
            // AABB
            body->shape = TP_SHAPE_AABB;
            body->w = 30 + (rand() % 40);
            body->h = 30 + (rand() % 40);
            body->restitution = 0.2f + ((float)rand() / RAND_MAX) * 0.3f;
        }
        
        // Random color
        bodyColors[i] = ColorToInt((Color){
            (unsigned char)(50 + rand() % 200),
            (unsigned char)(50 + rand() % 200),
            (unsigned char)(50 + rand() % 200),
            255
        });
        
        body->friction = 0.02f;
    }
    
    // Create static ground
    tp_body* ground = tp_world_add_body(&world);
    ground->x = SCREEN_WIDTH / 2;
    ground->y = SCREEN_HEIGHT - 20;
    ground->w = SCREEN_WIDTH - 40;
    ground->h = 40;
    ground->shape = TP_SHAPE_AABB;
    ground->restitution = 0.4f;
    tp_set_mass(ground, 0.0f); // Static body
    bodyColors[bodyCount++] = ColorToInt(GRAY);
    
    // Create static walls
    tp_body* leftWall = tp_world_add_body(&world);
    leftWall->x = 10;
    leftWall->y = SCREEN_HEIGHT / 2;
    leftWall->w = 20;
    leftWall->h = SCREEN_HEIGHT - 80;
    leftWall->shape = TP_SHAPE_AABB;
    leftWall->restitution = 0.4f;
    tp_set_mass(leftWall, 0.0f);
    bodyColors[bodyCount++] = ColorToInt(GRAY);
    
    tp_body* rightWall = tp_world_add_body(&world);
    rightWall->x = SCREEN_WIDTH - 10;
    rightWall->y = SCREEN_HEIGHT / 2;
    rightWall->w = 20;
    rightWall->h = SCREEN_HEIGHT - 80;
    rightWall->shape = TP_SHAPE_AABB;
    rightWall->restitution = 0.4f;
    tp_set_mass(rightWall, 0.0f);
    bodyColors[bodyCount++] = ColorToInt(GRAY);
    
    // Also use collision.h for debug visualization
    col_shape2d debugShapes[MAX_BODIES];
    
    // Main game loop
    while (!WindowShouldClose()) {
        // Add new body when space is pressed
        if (IsKeyPressed(KEY_SPACE) && world.count < MAX_BODIES) {
            tp_body* newBody = tp_world_add_body(&world);
            
            // Create at mouse position
            Vector2 mousePos = GetMousePosition();
            newBody->x = mousePos.x;
            newBody->y = mousePos.y;
            
            // Random velocity
            newBody->vx = (rand() % 100) - 50;
            newBody->vy = (rand() % 50) - 25;
            
            // Random properties
            float mass = 1.0f + (rand() % 10);
            tp_set_mass(newBody, mass);
            
            if (rand() % 2 == 0) {
                newBody->shape = TP_SHAPE_CIRCLE;
                newBody->radius = 15 + (rand() % 25);
                newBody->restitution = 0.3f + ((float)rand() / RAND_MAX) * 0.4f;
            } else {
                newBody->shape = TP_SHAPE_AABB;
                newBody->w = 30 + (rand() % 40);
                newBody->h = 30 + (rand() % 40);
                newBody->restitution = 0.2f + ((float)rand() / RAND_MAX) * 0.3f;
            }
            
            newBody->friction = 0.02f;
            
            // Random color
            bodyColors[world.count - 1] = ColorToInt((Color){
                (unsigned char)(50 + rand() % 200),
                (unsigned char)(50 + rand() % 200),
                (unsigned char)(50 + rand() % 200),
                255
            });
        }
        
        // Physics step
        tp_world_step(&world);
        
        // Update debug shapes for collision.h visualization
        for (int i = 0; i < world.count; i++) {
            tp_body* body = &world.bodies[i];
            debugShapes[i].x = body->x;
            debugShapes[i].y = body->y;
            debugShapes[i].w = body->w;
            debugShapes[i].h = body->h;
            debugShapes[i].r = body->radius;
            debugShapes[i].shape = body->shape;
        }
        
        // Draw
        BeginDrawing();
        ClearBackground(RAYWHITE);
        
        // Draw all bodies
        for (int i = 0; i < world.count; i++) {
            tp_body* body = &world.bodies[i];
            Color color = IntToColor(bodyColors[i]);
            
            if (body->shape == TP_SHAPE_CIRCLE) {
                DrawCircle((int)body->x, (int)body->y, body->radius, color);
                DrawCircleLines((int)body->x, (int)body->y, body->radius, DARKGRAY);
            } else {
                Rectangle rect = {
                    body->x - body->w / 2,
                    body->y - body->h / 2,
                    body->w,
                    body->h
                };
                DrawRectangleRec(rect, color);
                DrawRectangleLinesEx(rect, 2, DARKGRAY);
            }
            
            // Draw velocity vector
            if (body->inv_mass > 0) { // Only for dynamic bodies
                DrawLine(
                    (int)body->x, (int)body->y,
                    (int)(body->x + body->vx * 0.1f),
                    (int)(body->y + body->vy * 0.1f),
                    BLUE
                );
            }
        }
        
        // Draw collision.h debug info (show active collisions)
        for (int i = 0; i < world.count; i++) {
            for (int j = i + 1; j < world.count; j++) {
                col_manifold m;
                if (col_intersect_shape_manifold(&debugShapes[i], &debugShapes[j], &m)) {
                    // Draw collision normal
                    DrawLine(
                        (int)m.px, (int)m.py,
                        (int)(m.px + m.nx * 20),
                        (int)(m.py + m.ny * 20),
                        RED
                    );
                    
                    // Draw contact point
                    DrawCircle((int)m.px, (int)m.py, 3, RED);
                }
            }
        }
        
        // Draw UI
        DrawText("Physics Demo - TinyPhys2D + Collision.h", 10, 10, 20, DARKGRAY);
        DrawText("Press SPACE to add new body at mouse position", 10, 35, 15, DARKGRAY);
        DrawText(TextFormat("Bodies: %d/%d", world.count, MAX_BODIES), 10, 55, 15, DARKGRAY);
        DrawText("Blue lines: Velocity vectors", 10, 75, 15, DARKGRAY);
        DrawText("Red lines: Collision normals", 10, 95, 15, DARKGRAY);
        
        EndDrawing();
    }
    
    CloseWindow();
    return 0;
}
