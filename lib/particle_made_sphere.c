// particle_made_sphere.c
#define PARTICLES_IMPLEMENTATION
#include "gp3d_particles.h"
#include "raylib.h"
#include <math.h>

int main(void) {
  const int screenWidth = 1000;
  const int screenHeight = 600;

  InitWindow(screenWidth, screenHeight,
             "True 3D Particle Sphere - Volumetric Beauty");
  SetTargetFPS(60);

  // Camera setup
  Camera3D camera = {0};
  camera.position = (Vector3){0.0f, 0.0f, 6.0f};
  camera.target = (Vector3){0.0f, 0.0f, 0.0f};
  camera.up = (Vector3){0.0f, 1.0f, 0.0f};
  camera.fovy = 45.0f;
  camera.projection = CAMERA_PERSPECTIVE;

  // Create particle emitter - fewer particles for cleaner look
  ParticleEmitter3D sphere = PF3D_CreateEmitter(1500);
  sphere.position = (Vector3){0, 0, 0};
  sphere.sphereRadius = 2.0f;
  sphere.emitRate = 300; // Slower emission
  sphere.enabled = true;

  // Longer lifetime for stable globe appearance
  sphere.lifetimeMin = 8.0f;
  sphere.lifetimeMax = 12.0f;

  // Consistent, smaller particle sizes
  sphere.sizeStartMin = 1.0f;
  sphere.sizeStartMax = 1.5f;
  sphere.sizeEndMin = 0.8f;
  sphere.sizeEndMax = 1.2f;

  // More uniform, vibrant color
  sphere.colorStartMin = (Color){80, 160, 255, 220};
  sphere.colorStartMax = (Color){120, 200, 255, 240};
  sphere.colorEndMin = (Color){60, 140, 255, 180};
  sphere.colorEndMax = (Color){100, 180, 255, 200};

  // Minimal random velocity for stable appearance
  sphere.velMin = (Vector3){-0.05f, -0.05f, -0.05f};
  sphere.velMax = (Vector3){0.05f, 0.05f, 0.05f};

  // Gentle spinning motion
  float vortexStrength = 2.5f;
  PF3D_AddAffector(&sphere, PF3D_Affector_Vortex, &vortexStrength);

  // Strong drag to keep particles in place
  float drag = 0.985f;
  PF3D_AddAffector(&sphere, PF3D_Affector_Drag, &drag);

  while (!WindowShouldClose()) {
    float dt = GetFrameTime();

    // Gentle pulsing (optional - can disable for pure spinning globe)
    float pulse = 2.0f + sinf(GetTime() * 0.5f) * 0.1f;
    sphere.sphereRadius = pulse;

    // Orbit camera around sphere
    float time = GetTime();
    camera.position.x = sinf(time * 0.3f) * 6.0f;
    camera.position.z = cosf(time * 0.3f) * 6.0f;
    camera.position.y = sinf(time * 0.15f) * 2.0f; // Add vertical movement

    // Update particles
    PF3D_Update(&sphere, dt, &camera);

    // Render
    BeginDrawing();
    ClearBackground(BLACK);

    // Draw particles
    PF3D_Draw(&sphere, &camera);

    // UI
    DrawFPS(10, 10);
    DrawText("Spinning Globe - 3D Particle System", 10, 40, 20, WHITE);
    DrawText(TextFormat("Active Particles: %d", sphere.maxParticles), 10, 70,
             20, LIGHTGRAY);

    EndDrawing();
  }

  PF3D_FreeEmitter(&sphere);
  CloseWindow();
  return 0;
}
