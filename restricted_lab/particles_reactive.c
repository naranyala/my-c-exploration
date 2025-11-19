#include "lib/reactive.h"
#include "raylib.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// --- Domain ---

typedef struct {
  Vector2 position;
} TargetState;

typedef struct {
  Vector2 position;
  Color color;
  float speed;
} Particle;

// Now the observer receives the particle context!
void ParticleObserver(void *subjectData, void *observerContext) {
  TargetState *target = (TargetState *)subjectData;
  Particle *p = (Particle *)observerContext;

  // Move particle towards target
  Vector2 dir = {target->position.x - p->position.x,
                 target->position.y - p->position.y};
  float dist = sqrtf(dir.x * dir.x + dir.y * dir.y);

  if (dist > 1.0f) {
    dir.x /= dist;
    dir.y /= dist;
    p->position.x += dir.x * p->speed;
    p->position.y += dir.y * p->speed;
  }
}

int main(void) {
  const int screenWidth = 800;
  const int screenHeight = 600;

  InitWindow(screenWidth, screenHeight, "Reactive Stress Test: Particles");
  SetTargetFPS(60);

  TargetState target = {{screenWidth / 2.0f, screenHeight / 2.0f}};
  ReactiveState appState;
  InitReactiveState(&appState, &target);

  // Create many particles
  const int PARTICLE_COUNT = 1000;
  Particle *particles = (Particle *)malloc(PARTICLE_COUNT * sizeof(Particle));

  for (int i = 0; i < PARTICLE_COUNT; i++) {
    particles[i].position = (Vector2){GetRandomValue(0, screenWidth),
                                      GetRandomValue(0, screenHeight)};
    particles[i].color =
        (Color){GetRandomValue(50, 255), GetRandomValue(50, 255),
                GetRandomValue(50, 255), 200};
    particles[i].speed = GetRandomValue(2, 5);

    // Subscribe with CONTEXT!
    Subscribe(&appState, ParticleObserver, &particles[i]);
  }

  while (!WindowShouldClose()) {
    Vector2 mousePos = GetMousePosition();

    // Update target
    target.position = mousePos;

    // Notify all particles to update themselves based on new target
    // Note: In a real game loop, you'd probably update particles in a loop,
    // but here we are demonstrating the observer pattern driving the logic.
    SetState(&appState);

    BeginDrawing();
    ClearBackground(RAYWHITE);

    // Draw Particles
    for (int i = 0; i < PARTICLE_COUNT; i++) {
      DrawPixelV(particles[i].position, particles[i].color);
    }

    DrawCircleV(target.position, 10, MAROON);
    DrawText("1000 Observers tracking mouse!", 10, 10, 20, DARKGRAY);
    EndDrawing();
  }

  CleanupReactiveState(&appState);
  free(particles);
  CloseWindow();
  return 0;
}
