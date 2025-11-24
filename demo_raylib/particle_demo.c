#define PARTICLE_IMPLEMENTATION
#include "../lib/particle.h"
#include "raylib.h"
#include <time.h>


// Conversion functions between our types and raylib types
static inline Vector2 particle_vec2_to_raylib(ParticleVec2 v) { 
    return (Vector2){v.x, v.y}; 
}

static inline ParticleVec2 particle_vec2_from_raylib(Vector2 v) { 
    return (ParticleVec2){v.x, v.y}; 
}

static inline Color particle_color_to_raylib(ParticleColor c) { 
    return (Color){c.r, c.g, c.b, c.a}; 
}

static inline ParticleColor particle_color_from_raylib(Color c) { 
    return (ParticleColor){c.r, c.g, c.b, c.a}; 
}

// Raylib-specific renderer
void particle_system_draw_raylib(ParticleSystem* system) {
    for (int i = 0; i < system->maxParticles; i++) {
        Particle* p = &system->particles[i];
        if (!p->active) continue;
        
        // Calculate interpolated color
        float lifeRatio = 1.0f - (p->life / p->maxLife);
        Color start = particle_color_to_raylib(system->startColor);
        Color end = particle_color_to_raylib(system->endColor);
        Color particleColor = {
            (unsigned char)(start.r + (end.r - start.r) * lifeRatio),
            (unsigned char)(start.g + (end.g - start.g) * lifeRatio),
            (unsigned char)(start.b + (end.b - start.b) * lifeRatio),
            (unsigned char)(start.a + (end.a - start.a) * lifeRatio)
        };
        
        Vector2 pos = particle_vec2_to_raylib(p->position);
        
        // Draw simple circle
        DrawCircleV(pos, p->size / 2, particleColor);
    }
}

int main(void) {
    InitWindow(800, 600, "Compact Particle System");
    SetTargetFPS(60);
    
    // Initialize random seed
    srand(time(NULL));

    // Create particle system using our types
    ParticleSystem system = particle_system_create(500, particle_vec2_from_raylib((Vector2){400, 300}));
    
    // Configure using our color types
    particle_set_colors(&system, 
        particle_color_from_raylib((Color){255, 255, 0, 255}),    // YELLOW
        particle_color_from_raylib((Color){255, 0, 0, 0})        // RED with fade to transparent
    );
    particle_set_size_range(&system, 2.0f, 6.0f);
    particle_set_speed_range(&system, 50.0f, 150.0f);
    particle_set_life_range(&system, 0.5f, 2.0f);
    
    system.emitting = true;
    system.spawnRate = 20.0f;

    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();
        
        // Update with mouse position (convert from raylib to our type)
        system.origin = particle_vec2_from_raylib(GetMousePosition());
        particle_system_update(&system, deltaTime);

        BeginDrawing();
        ClearBackground(BLACK);
        
        // Draw using our raylib adapter
        particle_system_draw_raylib(&system);
        
        DrawText("Move mouse to control particles", 10, 10, 20, WHITE);
        DrawText(TextFormat("Active: %d", particle_active_count(system)), 10, 40, 20, WHITE);
        
        EndDrawing();
    }

    particle_system_destroy(&system);
    CloseWindow();
    return 0;
}
