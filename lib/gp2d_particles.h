
#ifndef PARTICLES_H
#define PARTICLES_H

#include "raylib.h"
#include <math.h>
#include <stdlib.h>

#ifndef PARTICLESDEF
#if defined(PARTICLES_STATIC)
#define PARTICLESDEF static
#else
#define PARTICLESDEF extern
#endif
#endif

// ---------------------------------------------------------------
// Emitter Shape Types
// ---------------------------------------------------------------
typedef enum { PF_EMIT_POINT, PF_EMIT_SPHERE } ParticleEmitShape;

// ---------------------------------------------------------------
// Particle + Emitter Types
// ---------------------------------------------------------------
typedef struct {
  bool active;
  float age, lifetime;

  Vector2 pos;
  Vector2 vel;
  Vector2 acc;

  float size, sizeEnd;
  Color color, colorEnd;
  float rot, rotSpeed;
} Particle;

typedef struct ParticleAffector {
  void (*apply)(Particle *p, float dt, void *userdata);
  void *userdata;
} ParticleAffector;

typedef struct {
  Vector2 position;

  float emitRate; // particles/sec
  float emitAccumulator;

  // Lifetime
  float lifetimeMin, lifetimeMax;

  // Sizes
  float sizeStartMin, sizeStartMax;
  float sizeEndMin, sizeEndMax;

  // Vel/acc ranges
  Vector2 velMin, velMax;
  Vector2 accMin, accMax;

  // Rotation range
  float rotSpeedMin, rotSpeedMax;

  // Colors
  Color colorStartMin, colorStartMax;
  Color colorEndMin, colorEndMax;

  // Shape
  ParticleEmitShape shape;
  float sphereRadius;

  bool enabled;

  // Affectors
  ParticleAffector *affectors;
  int affectorCount;

  // Pool
  Particle *pool;
  int maxParticles;

} ParticleEmitter;

// ---------------------------------------------------------------
// Utility Helpers
// ---------------------------------------------------------------
static inline float pf_randf(float a, float b) {
  return a + ((float)rand() / RAND_MAX) * (b - a);
}

static inline Color pf_lerp_color(Color a, Color b, float t) {
  return (Color){a.r + (b.r - a.r) * t, a.g + (b.g - a.g) * t,
                 a.b + (b.b - a.b) * t, a.a + (b.a - a.a) * t};
}

// Random point on unit sphere
static inline Vector3 pf_random_unit_sphere() {
  float u = ((float)rand() / RAND_MAX) * 2.0f - 1.0f;
  float theta = ((float)rand() / RAND_MAX) * 2.0f * PI;
  float f = sqrtf(1 - u * u);

  return (Vector3){f * cosf(theta), f * sinf(theta), u};
}

// ---------------------------------------------------------------
// API
// ---------------------------------------------------------------
PARTICLESDEF ParticleEmitter PF_CreateEmitter(int maxParticles);
PARTICLESDEF void PF_FreeEmitter(ParticleEmitter *e);

PARTICLESDEF void PF_UpdateEmitter(ParticleEmitter *e, float dt);
PARTICLESDEF void PF_DrawEmitter(const ParticleEmitter *e);

PARTICLESDEF void PF_AddAffector(ParticleEmitter *e,
                                 void (*fn)(Particle *, float, void *),
                                 void *userdata);

// Built-in Affectors
PARTICLESDEF void PF_Affector_Gravity(Particle *p, float dt, void *ud);
PARTICLESDEF void PF_Affector_LinearDrag(Particle *p, float dt, void *ud);
PARTICLESDEF void PF_Affector_SphereOrbit(Particle *p, float dt, void *ud);

#endif // PARTICLES_H

// ============================================================================
// IMPLEMENTATION
// ============================================================================
#ifdef PARTICLES_IMPLEMENTATION

PARTICLESDEF ParticleEmitter PF_CreateEmitter(int maxParticles) {
  ParticleEmitter e = {0};

  e.maxParticles = maxParticles;
  e.pool = calloc(maxParticles, sizeof(Particle));

  e.enabled = true;
  e.shape = PF_EMIT_POINT;
  e.sphereRadius = 0;

  return e;
}

PARTICLESDEF void PF_FreeEmitter(ParticleEmitter *e) {
  if (!e)
    return;

  free(e->pool);
  free(e->affectors);

  *e = (ParticleEmitter){0};
}

PARTICLESDEF void PF_AddAffector(ParticleEmitter *e,
                                 void (*fn)(Particle *, float, void *),
                                 void *userdata) {
  e->affectors =
      realloc(e->affectors, sizeof(ParticleAffector) * (e->affectorCount + 1));

  e->affectors[e->affectorCount].apply = fn;
  e->affectors[e->affectorCount].userdata = userdata;
  e->affectorCount++;
}

// ---------------------------------------------------------------
// Emission
// ---------------------------------------------------------------
static void pf_emit_particle(ParticleEmitter *e) {

  for (int i = 0; i < e->maxParticles; i++) {
    Particle *p = &e->pool[i];
    if (p->active)
      continue;

    p->active = true;
    p->age = 0;
    p->lifetime = pf_randf(e->lifetimeMin, e->lifetimeMax);

    // ---------------------------------------------------
    // Choose position based on shape
    // ---------------------------------------------------
    if (e->shape == PF_EMIT_POINT) {
      p->pos = e->position;
      p->vel.x = pf_randf(e->velMin.x, e->velMax.x);
      p->vel.y = pf_randf(e->velMin.y, e->velMax.y);
    } else if (e->shape == PF_EMIT_SPHERE) {
      Vector3 s = pf_random_unit_sphere();
      Vector3 scaled = {s.x * e->sphereRadius, s.y * e->sphereRadius,
                        s.z * e->sphereRadius};

      p->pos.x = e->position.x + scaled.x;
      p->pos.y = e->position.y + scaled.y * 0.6f;

      p->vel.x = scaled.x;
      p->vel.y = scaled.y;
    }

    p->acc.x = pf_randf(e->accMin.x, e->accMax.x);
    p->acc.y = pf_randf(e->accMin.y, e->accMax.y);

    p->size = pf_randf(e->sizeStartMin, e->sizeStartMax);
    p->sizeEnd = pf_randf(e->sizeEndMin, e->sizeEndMax);

    p->rotSpeed = pf_randf(e->rotSpeedMin, e->rotSpeedMax);

    p->color = (Color){
        pf_randf(e->colorStartMin.r, e->colorStartMax.r),
        pf_randf(e->colorStartMin.g, e->colorStartMax.g),
        pf_randf(e->colorStartMin.b, e->colorStartMax.b),
        pf_randf(e->colorStartMin.a, e->colorStartMax.a),
    };

    p->colorEnd = (Color){
        pf_randf(e->colorEndMin.r, e->colorEndMax.r),
        pf_randf(e->colorEndMin.g, e->colorEndMax.g),
        pf_randf(e->colorEndMin.b, e->colorEndMax.b),
        pf_randf(e->colorEndMin.a, e->colorEndMax.a),
    };

    return;
  }
}

// ---------------------------------------------------------------
// Update
// ---------------------------------------------------------------
PARTICLESDEF void PF_UpdateEmitter(ParticleEmitter *e, float dt) {
  if (!e->enabled)
    return;

  // Emit particles
  e->emitAccumulator += dt * e->emitRate;
  while (e->emitAccumulator >= 1.0f) {
    pf_emit_particle(e);
    e->emitAccumulator -= 1.0f;
  }

  for (int i = 0; i < e->maxParticles; i++) {
    Particle *p = &e->pool[i];
    if (!p->active)
      continue;

    p->age += dt;
    float t = p->age / p->lifetime;

    if (t >= 1.0f) {
      p->active = false;
      continue;
    }

    p->vel.x += p->acc.x * dt;
    p->vel.y += p->acc.y * dt;

    p->pos.x += p->vel.x * dt;
    p->pos.y += p->vel.y * dt;

    p->rot += p->rotSpeed * dt;

    p->size = p->size + (p->sizeEnd - p->size) * t;
    p->color = pf_lerp_color(p->color, p->colorEnd, t);

    // Affectors
    for (int a = 0; a < e->affectorCount; a++)
      e->affectors[a].apply(p, dt, e->affectors[a].userdata);
  }
}

// ---------------------------------------------------------------
// Draw
// ---------------------------------------------------------------
PARTICLESDEF void PF_DrawEmitter(const ParticleEmitter *e) {
  for (int i = 0; i < e->maxParticles; i++) {
    const Particle *p = &e->pool[i];
    if (!p->active)
      continue;

    DrawCircleV(p->pos, p->size, p->color);
  }
}

// ---------------------------------------------------------------
// Built-in Affectors
// ---------------------------------------------------------------
PARTICLESDEF void PF_Affector_Gravity(Particle *p, float dt, void *ud) {
  Vector2 *g = ud;
  p->vel.x += g->x * dt;
  p->vel.y += g->y * dt;
}

PARTICLESDEF void PF_Affector_LinearDrag(Particle *p, float dt, void *ud) {
  float drag = *(float *)ud;
  p->vel.x *= (1.0f - drag * dt);
  p->vel.y *= (1.0f - drag * dt);
}

PARTICLESDEF void PF_Affector_SphereOrbit(Particle *p, float dt, void *ud) {
  float speed = *(float *)ud;

  float vx = p->vel.x;
  float vy = p->vel.y;

  p->vel.x = vx * cosf(speed * dt) - vy * sinf(speed * dt);
  p->vel.y = vx * sinf(speed * dt) + vy * cosf(speed * dt);
}

#endif // PARTICLES_IMPLEMENTATION
