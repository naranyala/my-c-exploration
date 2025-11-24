#ifndef PARTICLE_H
#define PARTICLE_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Platform-agnostic types with distinct names
typedef struct { float x, y; } ParticleVec2;
typedef struct { unsigned char r, g, b, a; } ParticleColor;
typedef struct { 
    int id; 
    int width; 
    int height; 
    void* data; 
} ParticleTexture;

// Particle structure
typedef struct {
    ParticleVec2 position, velocity, acceleration;
    ParticleColor color;
    float size, life, maxLife, rotation, rotationSpeed;
    bool active;
} Particle;

// Particle system
typedef struct {
    Particle* particles;
    int maxParticles, activeCount;
    ParticleVec2 origin;
    ParticleColor startColor, endColor;
    float minSize, maxSize, minSpeed, maxSpeed, minLife, maxLife;
    float spawnRate, spawnTimer;
    bool emitting;
    ParticleTexture texture;
    bool useTexture;
} ParticleSystem;

// Core API
ParticleSystem particle_system_create(int maxParticles, ParticleVec2 origin);
void particle_system_update(ParticleSystem* system, float deltaTime);
void particle_system_destroy(ParticleSystem* system);
void particle_system_reset(ParticleSystem* system);
void particle_spawn(ParticleSystem* system);
void particle_spawn_ex(ParticleSystem* system, ParticleVec2 position, ParticleVec2 direction);

// Configuration
void particle_set_colors(ParticleSystem* system, ParticleColor start, ParticleColor end);
void particle_set_size_range(ParticleSystem* system, float min, float max);
void particle_set_speed_range(ParticleSystem* system, float min, float max);
void particle_set_life_range(ParticleSystem* system, float min, float max);
void particle_set_texture(ParticleSystem* system, ParticleTexture texture);

// Utility
int particle_active_count(ParticleSystem system);

#ifdef PARTICLE_IMPLEMENTATION

#include <stdlib.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

// Proper random float functions
static inline float particle_rand_float(float min, float max) {
    return min + (max - min) * ((float)rand() / (float)RAND_MAX);
}

ParticleSystem particle_system_create(int maxParticles, ParticleVec2 origin) {
    ParticleSystem system = {0};
    system.particles = (Particle*)malloc(sizeof(Particle) * maxParticles);
    system.maxParticles = maxParticles;
    system.origin = origin;
    system.startColor = (ParticleColor){255, 255, 255, 255};
    system.endColor = (ParticleColor){255, 255, 255, 255};
    system.minSize = 2.0f; 
    system.maxSize = 8.0f;
    system.minSpeed = 50.0f; 
    system.maxSpeed = 200.0f;
    system.minLife = 1.0f; 
    system.maxLife = 3.0f;
    system.spawnRate = 10.0f;
    system.emitting = false;
    system.useTexture = false;
    system.texture = (ParticleTexture){0};
    
    for (int i = 0; i < maxParticles; i++) {
        system.particles[i].active = false;
    }
    
    return system;
}

void particle_system_update(ParticleSystem* system, float deltaTime) {
    if (!system) return;
    
    // Spawn particles
    if (system->emitting) {
        system->spawnTimer += deltaTime;
        float timeBetweenSpawns = 1.0f / system->spawnRate;
        
        while (system->spawnTimer >= timeBetweenSpawns && system->activeCount < system->maxParticles) {
            particle_spawn(system);
            system->spawnTimer -= timeBetweenSpawns;
        }
    }
    
    // Update particles
    for (int i = 0; i < system->maxParticles; i++) {
        Particle* p = &system->particles[i];
        if (!p->active) continue;
        
        p->velocity.x += p->acceleration.x * deltaTime;
        p->velocity.y += p->acceleration.y * deltaTime;
        p->position.x += p->velocity.x * deltaTime;
        p->position.y += p->velocity.y * deltaTime;
        p->rotation += p->rotationSpeed * deltaTime;
        
        p->life -= deltaTime;
        if (p->life <= 0.0f) {
            p->active = false;
            system->activeCount--;
        }
    }
}

void particle_system_destroy(ParticleSystem* system) {
    if (system && system->particles) {
        free(system->particles);
        system->particles = NULL;
        system->maxParticles = 0;
    }
}

void particle_system_reset(ParticleSystem* system) {
    if (system) {
        for (int i = 0; i < system->maxParticles; i++) {
            system->particles[i].active = false;
        }
        system->activeCount = 0;
        system->spawnTimer = 0.0f;
    }
}

void particle_spawn(ParticleSystem* system) {
    particle_spawn_ex(system, system->origin, (ParticleVec2){0, 0});
}

void particle_spawn_ex(ParticleSystem* system, ParticleVec2 position, ParticleVec2 direction) {
    if (!system || system->activeCount >= system->maxParticles) return;
    
    for (int i = 0; i < system->maxParticles; i++) {
        Particle* p = &system->particles[i];
        if (!p->active) {
            p->active = true;
            p->position = position;
            
            if (direction.x == 0 && direction.y == 0) {
                float angle = particle_rand_float(0, 2 * M_PI);
                float speed = particle_rand_float(system->minSpeed, system->maxSpeed);
                p->velocity = (ParticleVec2){cosf(angle) * speed, sinf(angle) * speed};
            } else {
                float speed = particle_rand_float(system->minSpeed, system->maxSpeed);
                p->velocity = (ParticleVec2){direction.x * speed, direction.y * speed};
            }
            
            p->acceleration = (ParticleVec2){0, 98.0f}; // Gravity
            p->size = particle_rand_float(system->minSize, system->maxSize);
            p->maxLife = particle_rand_float(system->minLife, system->maxLife);
            p->life = p->maxLife;
            p->rotation = particle_rand_float(0, 360);
            p->rotationSpeed = particle_rand_float(-180, 180);
            p->color = system->startColor;
            
            system->activeCount++;
            break;
        }
    }
}

void particle_set_colors(ParticleSystem* system, ParticleColor start, ParticleColor end) {
    if (system) { 
        system->startColor = start; 
        system->endColor = end; 
    }
}

void particle_set_size_range(ParticleSystem* system, float min, float max) {
    if (system) { 
        system->minSize = min; 
        system->maxSize = max; 
    }
}

void particle_set_speed_range(ParticleSystem* system, float min, float max) {
    if (system) { 
        system->minSpeed = min; 
        system->maxSpeed = max; 
    }
}

void particle_set_life_range(ParticleSystem* system, float min, float max) {
    if (system) { 
        system->minLife = min; 
        system->maxLife = max; 
    }
}

void particle_set_texture(ParticleSystem* system, ParticleTexture texture) {
    if (system) { 
        system->texture = texture; 
        system->useTexture = true; 
    }
}

int particle_active_count(ParticleSystem system) {
    return system.activeCount;
}

#endif // PARTICLE_IMPLEMENTATION

#ifdef __cplusplus
}
#endif

#endif // PARTICLE_H
