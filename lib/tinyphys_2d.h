
// tinyphys_2d.h
// Minimal single-header 2D physics for C.
// - Semi-implicit Euler integration
// - Gravity, damping
// - Circle-circle collision with restitution
// - Simple world stepping (fixed dt recommended)

#include <math.h>

#ifndef TINY_PHYS_H
#define TINY_PHYS_H

#ifdef __cplusplus
extern "C" {
#endif

// =====================
// Configurable constants
// =====================
#ifndef TP_MAX_BODIES
#define TP_MAX_BODIES 1024
#endif

// =====================
// Types
// =====================

typedef struct {
    float x, y;
} tp_vec2;

typedef struct {
    tp_vec2 pos;
    tp_vec2 vel;
    tp_vec2 force;      // Accumulated per-step
    float   radius;     // Circle shape
    float   mass;       // > 0; infinite mass via is_static
    float   inv_mass;   // Cached; 0 for static bodies
    float   restitution;// [0..1] bounciness
    float   damping;    // [0..1] per-second linear damping
    int     is_static;  // static bodies don't move
    int     alive;      // for simple management
    int     id;         // user-facing id
} tp_body;

typedef struct {
    tp_body bodies[TP_MAX_BODIES];
    int count;
    tp_vec2 gravity;     // world gravity (e.g., {0, 9.8f})
} tp_world;

// =====================
// Helpers
// =====================

static inline tp_vec2 tp_v2(float x, float y) { tp_vec2 v = {x,y}; return v; }

static inline tp_vec2 tp_add(tp_vec2 a, tp_vec2 b) { return tp_v2(a.x+b.x, a.y+b.y); }
static inline tp_vec2 tp_sub(tp_vec2 a, tp_vec2 b) { return tp_v2(a.x-b.x, a.y-b.y); }
static inline tp_vec2 tp_scale(tp_vec2 a, float s) { return tp_v2(a.x*s, a.y*s); }
static inline float   tp_dot(tp_vec2 a, tp_vec2 b) { return a.x*b.x + a.y*b.y; }
static inline float   tp_len2(tp_vec2 a) { return tp_dot(a,a); }
static inline float   tp_len(tp_vec2 a) { 
    float l2 = tp_len2(a); 
    return l2 > 0 ? (float)sqrtf(l2) : 0.0f; 
}
static inline tp_vec2 tp_norm(tp_vec2 a) {
    float l = tp_len(a);
    return (l > 1e-8f) ? tp_scale(a, 1.0f/l) : tp_v2(0.0f, 0.0f);
}

// =====================
// API
// =====================

void tp_world_init(tp_world* w, tp_vec2 gravity);
int  tp_body_add(tp_world* w,
                 tp_vec2 pos, tp_vec2 vel,
                 float radius, float mass,
                 float restitution, float damping,
                 int is_static);
void tp_body_remove(tp_world* w, int id);

void tp_apply_force(tp_world* w, int id, tp_vec2 f);
void tp_apply_impulse(tp_world* w, int id, tp_vec2 j);

void tp_step(tp_world* w, float dt);

// Utility getters (optional)
tp_body* tp_get_body(tp_world* w, int id);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // TINY_PHYS_H


#ifdef TINY_PHYS_IMPLEMENTATION
#include <math.h>
#include <string.h>

// ===== Internal helpers =====
static void tp_integrate(tp_body* b, tp_vec2 gravity, float dt) {
    if (!b->alive || b->is_static) return;

    // Accumulate gravity
    tp_vec2 acc = tp_scale(gravity, b->inv_mass);
    acc = tp_add(acc, tp_scale(b->force, b->inv_mass));

    // Semi-implicit Euler: v += a*dt; p += v*dt
    b->vel = tp_add(b->vel, tp_scale(acc, dt));

    // Apply exponential damping: v *= exp(-damping*dt)
    // Treat damping as per-second factor
    float damp = b->damping;
    if (damp > 0.0f) {
        float factor = expf(-damp * dt);
        b->vel = tp_scale(b->vel, factor);
    }

    b->pos = tp_add(b->pos, tp_scale(b->vel, dt));

    // Clear per-step force
    b->force = tp_v2(0.0f, 0.0f);
}

static void tp_resolve_pair(tp_body* a, tp_body* b) {
    if (!a->alive || !b->alive) return;

    tp_vec2 n = tp_sub(b->pos, a->pos);
    float dist = tp_len(n);
    float rsum = a->radius + b->radius;

    if (dist <= 0.0f) {
        // Overlapping at same position: pick arbitrary normal
        n = tp_v2(1.0f, 0.0f);
        dist = 1e-6f;
    }

    if (dist < rsum) {
        tp_vec2 normal = tp_scale(n, 1.0f / dist);
        float penetration = rsum - dist;

        // Positional correction (split by inverse mass; static bodies get none)
        float invA = a->inv_mass;
        float invB = b->inv_mass;
        float invSum = invA + invB;

        if (invSum > 0.0f) {
            float percent = 0.8f;     // penetration correction strength
            float slop    = 0.001f;   // tolerance
            float corrMag = (penetration - slop) > 0.0f ? percent * (penetration - slop) : 0.0f;
            tp_vec2 correction = tp_scale(normal, corrMag);

            if (!a->is_static) a->pos = tp_sub(a->pos, tp_scale(correction, invA / invSum));
            if (!b->is_static) b->pos = tp_add(b->pos, tp_scale(correction, invB / invSum));
        }

        // Relative velocity
        tp_vec2 rv = tp_sub(b->vel, a->vel);
        float velAlongNormal = tp_dot(rv, normal);

        // Only resolve if bodies are moving toward each other
        if (velAlongNormal < 0.0f) {
            float e = fmaxf(0.0f, fminf(1.0f, (a->restitution + b->restitution) * 0.5f));
            float j = -(1.0f + e) * velAlongNormal;
            float denom = invA + invB;
            if (denom > 0.0f) j /= denom;

            tp_vec2 impulse = tp_scale(normal, j);

            if (!a->is_static) a->vel = tp_sub(a->vel, tp_scale(impulse, invA));
            if (!b->is_static) b->vel = tp_add(b->vel, tp_scale(impulse, invB));
        }
    }
}

// ===== Public API =====
void tp_world_init(tp_world* w, tp_vec2 gravity) {
    w->count = 0;
    w->gravity = gravity;
    for (int i = 0; i < TP_MAX_BODIES; ++i) {
        w->bodies[i].alive = 0;
        w->bodies[i].id = i;
    }
}

tp_body* tp_get_body(tp_world* w, int id) {
    if (id < 0 || id >= TP_MAX_BODIES) return NULL;
    tp_body* b = &w->bodies[id];
    return b->alive ? b : NULL;
}

int tp_body_add(tp_world* w,
                tp_vec2 pos, tp_vec2 vel,
                float radius, float mass,
                float restitution, float damping,
                int is_static) {
    // Find free slot
    for (int i = 0; i < TP_MAX_BODIES; ++i) {
        if (!w->bodies[i].alive) {
            tp_body* b = &w->bodies[i];
            b->pos = pos;
            b->vel = vel;
            b->force = tp_v2(0.0f, 0.0f);
            b->radius = radius > 0.0f ? radius : 0.5f;
            b->mass = is_static ? 0.0f : (mass > 0.0f ? mass : 1.0f);
            b->inv_mass = (b->mass > 0.0f) ? 1.0f / b->mass : 0.0f;
            b->restitution = restitution;
            b->damping = damping;
            b->is_static = is_static ? 1 : 0;
            b->alive = 1;
            b->id = i;
            w->count++;
            return i;
        }
    }
    return -1; // world full
}

void tp_body_remove(tp_world* w, int id) {
    tp_body* b = tp_get_body(w, id);
    if (!b) return;
    b->alive = 0;
    w->count--;
}

void tp_apply_force(tp_world* w, int id, tp_vec2 f) {
    tp_body* b = tp_get_body(w, id);
    if (!b || b->is_static) return;
    b->force = tp_add(b->force, f);
}

void tp_apply_impulse(tp_world* w, int id, tp_vec2 j) {
    tp_body* b = tp_get_body(w, id);
    if (!b || b->is_static) return;
    b->vel = tp_add(b->vel, tp_scale(j, b->inv_mass));
}

void tp_step(tp_world* w, float dt) {
    if (dt <= 0.0f) return;

    // Integrate motion
    for (int i = 0; i < TP_MAX_BODIES; ++i) {
        tp_body* b = &w->bodies[i];
        if (!b->alive) continue;
        tp_integrate(b, w->gravity, dt);
    }

    // Broadphase: naive O(n^2) for simplicity
    for (int i = 0; i < TP_MAX_BODIES; ++i) {
        tp_body* a = &w->bodies[i];
        if (!a->alive) continue;
        for (int j = i + 1; j < TP_MAX_BODIES; ++j) {
            tp_body* c = &w->bodies[j];
            if (!c->alive) continue;

            // Early out by distance check
            float rsum = a->radius + c->radius;
            tp_vec2 d = tp_sub(c->pos, a->pos);
            float d2 = tp_len2(d);
            if (d2 <= rsum * rsum) {
                tp_resolve_pair(a, c);
            }
        }
    }
}

#endif // TINY_PHYS_IMPLEMENTATION
