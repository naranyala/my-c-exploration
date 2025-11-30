// gp3d_particles.h
#ifndef GP3D_PARTICLES_H
#define GP3D_PARTICLES_H

#include "raylib.h"
#include "raymath.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

#ifndef PARTICLESDEF
#if defined(PARTICLES_STATIC)
#define PARTICLESDEF static
#else
#define PARTICLESDEF extern
#endif
#endif

// ---------------------------------------------------------------
// 3D Particle
// ---------------------------------------------------------------
typedef struct {
  bool active;
  float age, lifetime;

  Vector3 pos;
  Vector3 vel;
  Vector3 acc;

  float size, sizeEnd;
  Color color, colorEnd;
} Particle3D;

typedef struct ParticleAffector {
  void (*apply)(Particle3D *p, float dt, void *userdata);
  void *userdata;
} ParticleAffector;

// ---------------------------------------------------------------
// Emitter
// ---------------------------------------------------------------
typedef struct {
  Vector3 position;

  float emitRate;
  float emitAccumulator;

  float lifetimeMin, lifetimeMax;

  float sizeStartMin, sizeStartMax;
  float sizeEndMin, sizeEndMax;

  Vector3 velMin, velMax;
  Vector3 accMin, accMax;

  Color colorStartMin, colorStartMax;
  Color colorEndMin, colorEndMax;

  float sphereRadius;

  bool enabled;

  ParticleAffector *affectors;
  int affectorCount;

  Particle3D *pool;
  int maxParticles;
} ParticleEmitter3D;

// ---------------------------------------------------------------
// Utility
// ---------------------------------------------------------------
static inline float pf_randf(float a, float b) {
  return a + ((float)rand() / RAND_MAX) * (b - a);
}

static inline Color pf_lerp_color(Color a, Color b, float t) {
  return (Color){(unsigned char)(a.r + (b.r - a.r) * t),
                 (unsigned char)(a.g + (b.g - a.g) * t),
                 (unsigned char)(a.b + (b.b - a.b) * t),
                 (unsigned char)(a.a + (b.a - a.a) * t)};
}

// Random point inside unit sphere (uniform distribution)
static inline Vector3 pf_random_in_sphere() {
  Vector3 v;
  do {
    v.x = pf_randf(-1, 1);
    v.y = pf_randf(-1, 1);
    v.z = pf_randf(-1, 1);
  } while (v.x * v.x + v.y * v.y + v.z * v.z > 1.0f);
  return v;
}

// ---------------------------------------------------------------
// API
// ---------------------------------------------------------------
PARTICLESDEF ParticleEmitter3D PF3D_CreateEmitter(int maxParticles);
PARTICLESDEF void PF3D_FreeEmitter(ParticleEmitter3D *e);

PARTICLESDEF void PF3D_Update(ParticleEmitter3D *e, float dt, Camera *cam);
PARTICLESDEF void PF3D_Draw(ParticleEmitter3D *e, Camera *cam);

PARTICLESDEF void PF3D_AddAffector(ParticleEmitter3D *e,
                                   void (*fn)(Particle3D *, float, void *),
                                   void *userdata);

// Built-in Affectors
PARTICLESDEF void PF3D_Affector_Gravity(Particle3D *p, float dt, void *ud);
PARTICLESDEF void PF3D_Affector_Drag(Particle3D *p, float dt, void *ud);
PARTICLESDEF void PF3D_Affector_Vortex(Particle3D *p, float dt, void *ud);

#endif // GP3D_PARTICLES_H

#ifdef PARTICLES_IMPLEMENTATION

PARTICLESDEF ParticleEmitter3D PF3D_CreateEmitter(int maxParticles) {
  ParticleEmitter3D e = {0};
  e.maxParticles = maxParticles;
  e.pool = calloc(maxParticles, sizeof(Particle3D));
  e.enabled = true;
  e.sphereRadius = 1.0f;
  e.emitRate = 100.0f;
  e.lifetimeMin = 1.0f;
  e.lifetimeMax = 2.0f;
  return e;
}

PARTICLESDEF void PF3D_FreeEmitter(ParticleEmitter3D *e) {
  if (!e)
    return;
  free(e->pool);
  free(e->affectors);
  memset(e, 0, sizeof(ParticleEmitter3D));
}

PARTICLESDEF void PF3D_AddAffector(ParticleEmitter3D *e,
                                   void (*fn)(Particle3D *, float, void *),
                                   void *userdata) {
  e->affectors =
      realloc(e->affectors, sizeof(ParticleAffector) * (e->affectorCount + 1));
  e->affectors[e->affectorCount] = (ParticleAffector){fn, userdata};
  e->affectorCount++;
}

// Emit new particle inside sphere
static void emit_particle(ParticleEmitter3D *e) {
  for (int i = 0; i < e->maxParticles; i++) {
    Particle3D *p = &e->pool[i];
    if (p->active)
      continue;

    p->active = true;
    p->age = 0;
    p->lifetime = pf_randf(e->lifetimeMin, e->lifetimeMax);

    // Emit uniformly inside the sphere
    Vector3 offset = pf_random_in_sphere();
    offset = Vector3Scale(offset, e->sphereRadius);

    // Store world position directly
    p->pos = Vector3Add(e->position, offset);

    // Initial velocity (slight outward + random)
    p->vel = (Vector3){pf_randf(e->velMin.x, e->velMax.x),
                       pf_randf(e->velMin.y, e->velMax.y),
                       pf_randf(e->velMin.z, e->velMax.z)};

    p->acc = (Vector3){0, 0, 0};

    p->size = pf_randf(e->sizeStartMin, e->sizeStartMax);
    p->sizeEnd = pf_randf(e->sizeEndMin, e->sizeEndMax);

    p->color = (Color){
        (unsigned char)pf_randf(e->colorStartMin.r, e->colorStartMax.r),
        (unsigned char)pf_randf(e->colorStartMin.g, e->colorStartMax.g),
        (unsigned char)pf_randf(e->colorStartMin.b, e->colorStartMax.b),
        (unsigned char)pf_randf(e->colorStartMin.a, e->colorStartMax.a)};

    p->colorEnd =
        (Color){(unsigned char)pf_randf(e->colorEndMin.r, e->colorEndMax.r),
                (unsigned char)pf_randf(e->colorEndMin.g, e->colorEndMax.g),
                (unsigned char)pf_randf(e->colorEndMin.b, e->colorEndMax.b),
                (unsigned char)pf_randf(e->colorEndMin.a, e->colorEndMax.a)};

    return;
  }
}

PARTICLESDEF void PF3D_Update(ParticleEmitter3D *e, float dt, Camera *cam) {
  if (!e->enabled)
    return;

  // Emit particles
  e->emitAccumulator += dt * e->emitRate;
  while (e->emitAccumulator >= 1.0f) {
    emit_particle(e);
    e->emitAccumulator -= 1.0f;
  }

  // Update particles
  for (int i = 0; i < e->maxParticles; i++) {
    Particle3D *p = &e->pool[i];
    if (!p->active)
      continue;

    p->age += dt;
    float t = p->age / p->lifetime;
    if (t >= 1.0f) {
      p->active = false;
      continue;
    }

    // Apply affectors
    for (int a = 0; a < e->affectorCount; a++)
      e->affectors[a].apply(p, dt, e->affectors[a].userdata);

    // Physics
    p->vel = Vector3Add(p->vel, Vector3Scale(p->acc, dt));
    p->pos = Vector3Add(p->pos, Vector3Scale(p->vel, dt));

    // Interpolate size and color
    p->size = Lerp(p->size, p->sizeEnd, t);
    p->color = pf_lerp_color(p->color, p->colorEnd, t);
  }
}

// Depth sorting + perspective drawing
PARTICLESDEF void PF3D_Draw(ParticleEmitter3D *e, Camera *cam) {
  typedef struct {
    Particle3D *p;
    float depth;
    Vector2 screen;
  } SortedParticle;

  SortedParticle *sorted = calloc(e->maxParticles, sizeof(SortedParticle));
  int count = 0;

  // Collect active particles
  for (int i = 0; i < e->maxParticles; i++) {
    if (!e->pool[i].active)
      continue;

    Particle3D *p = &e->pool[i];

    // Calculate depth (distance from camera)
    float depth = Vector3Distance(cam->position, p->pos);

    // Project to screen
    Vector2 screen = GetWorldToScreen(p->pos, *cam);

    sorted[count++] = (SortedParticle){p, depth, screen};
  }

  // Sort back to front (painter's algorithm)
  for (int i = 0; i < count - 1; i++) {
    for (int j = 0; j < count - i - 1; j++) {
      if (sorted[j].depth < sorted[j + 1].depth) {
        SortedParticle temp = sorted[j];
        sorted[j] = sorted[j + 1];
        sorted[j + 1] = temp;
      }
    }
  }

  // Draw sorted particles
  for (int i = 0; i < count; i++) {
    Particle3D *p = sorted[i].p;
    Vector2 screen = sorted[i].screen;

    // Skip off-screen particles
    if (screen.x < -100 || screen.x > GetScreenWidth() + 100 ||
        screen.y < -100 || screen.y > GetScreenHeight() + 100)
      continue;

    // Perspective scale (closer = bigger)
    float depth = sorted[i].depth;
    float scale = 5.0f / (depth + 2.0f);

    DrawCircle((int)screen.x, (int)screen.y, p->size * scale * 10.0f, p->color);
  }

  free(sorted);
}

// Built-in Affectors
PARTICLESDEF void PF3D_Affector_Gravity(Particle3D *p, float dt, void *ud) {
  Vector3 *g = (Vector3 *)ud;
  p->vel = Vector3Add(p->vel, Vector3Scale(*g, dt));
}

PARTICLESDEF void PF3D_Affector_Drag(Particle3D *p, float dt, void *ud) {
  float *d = (float *)ud;
  p->vel = Vector3Scale(p->vel, *d);
}

PARTICLESDEF void PF3D_Affector_Vortex(Particle3D *p, float dt, void *ud) {
  float strength = *(float *)ud;

  // Use cross product for proper 3D rotation around Y axis
  Vector3 axis = {0, 1, 0}; // rotation axis (up)
  Vector3 r = p->pos;       // position relative to origin

  // Cross product gives perpendicular vector for circular motion
  Vector3 tangent = Vector3CrossProduct(axis, r);

  float len = Vector3Length(tangent);
  if (len > 0.001f) {
    tangent = Vector3Scale(Vector3Normalize(tangent), strength);
    p->vel = Vector3Add(p->vel, Vector3Scale(tangent, dt));
  }
}

#endif // PARTICLES_IMPLEMENTATION
