#define PARTICLES_IMPLEMENTATION
#include "gp2d_particles.h"
#include "raylib.h"

int main() {
  InitWindow(800, 450, "Particles Demo");
  SetTargetFPS(60);

  ParticleEmitter fire = PF_CreateEmitter(500);
  fire.position = (Vector2){400, 350};
  fire.emitRate = 120;

  fire.lifetimeMin = 0.6f;
  fire.lifetimeMax = 1.2f;

  fire.sizeStartMin = 6;
  fire.sizeStartMax = 12;
  fire.sizeEndMin = 0;
  fire.sizeEndMax = 0;

  fire.velMin = (Vector2){-20, -140};
  fire.velMax = (Vector2){20, -70};

  fire.colorStartMin = (Color){255, 120, 20, 200};
  fire.colorStartMax = (Color){255, 200, 40, 255};

  fire.colorEndMin = (Color){100, 20, 0, 0};
  fire.colorEndMax = (Color){150, 50, 0, 0};

  // Add gravity affector
  Vector2 gravity = {0, 80};
  PF_AddAffector(&fire, PF_Affector_Gravity, &gravity);

  float drag = 0.4f;
  PF_AddAffector(&fire, PF_Affector_LinearDrag, &drag);

  while (!WindowShouldClose()) {
    fire.position = GetMousePosition();

    PF_UpdateEmitter(&fire, GetFrameTime());

    BeginDrawing();
    ClearBackground(BLACK);

    PF_DrawEmitter(&fire);

    EndDrawing();
  }

  PF_FreeEmitter(&fire);
  CloseWindow();
}
