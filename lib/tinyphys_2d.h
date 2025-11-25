/* tinyphys_2d.h
 *
 * Self-contained minimal 2D physics library.
 * - Uses tp_ prefix for all public symbols.
 * - Contains bodies, world, integrator (semi-implicit Euler), simple constraints,
 *   basic broadphase (naive), pairwise collision detection (circle/AABB),
 *   simple positional correction and impulse-based response.
 *
 * No dependency on any other header.
 *
 * Usage:
 *   #include "tinyphys_2d.h"
 *   tp_body bodies[128];
 *   tp_world world;
 *   tp_world_init(&world, bodies, 128, 1.0f/60.0f);
 *   ...
 */

#ifndef TINYPHYS_2D_H
#define TINYPHYS_2D_H

#include <math.h>
#include <string.h> /* memset */
#include <stdlib.h> /* NULL */

#ifdef __cplusplus
extern "C" {
#endif

/* --- shapes and enums --- */
typedef enum {
    TP_SHAPE_CIRCLE = 0,
    TP_SHAPE_AABB   = 1
} tp_shape_type;

/* lightweight shape descriptor internal to tinyphys */
typedef struct {
    float x, y;    /* center */
    float w, h;    /* full width/height for AABB */
    float r;       /* radius for circle */
    int shape;     /* tp_shape_type */
} tp_shape2d;

/* --- body --- */
typedef struct {
    float x, y;
    float vx, vy;
    float ax, ay;      /* accumulated acceleration (forces) */
    float mass;
    float inv_mass;
    float radius;      /* for TP_SHAPE_CIRCLE */
    float w, h;        /* for TP_SHAPE_AABB */
    int shape;         /* tp_shape_type */
    float restitution; /* 0..1 */
    float friction;    /* linear damping */
    int sleeping;      /* boolean */
} tp_body;

/* --- world --- */
typedef struct {
    tp_body *bodies;
    int count;
    int capacity;
    float gravity_x, gravity_y;
    float dt;
} tp_world;

/* --- manifold (internal) --- */
typedef struct {
    float nx, ny;   /* normal A -> B */
    float depth;    /* penetration depth */
    float px, py;   /* contact point */
} tp_manifold;

/* --- helpers --- */
static inline void tp_init_body(tp_body *b) {
    if (!b) return;
    memset(b, 0, sizeof(*b));
    b->mass = 1.0f;
    b->inv_mass = 1.0f;
    b->restitution = 0.2f;
    b->friction = 0.02f;
}

static inline void tp_set_mass(tp_body *b, float m) {
    if (!b) return;
    b->mass = (m > 0.0f) ? m : 1.0f;
    b->inv_mass = 1.0f / b->mass;
}

static inline void tp_apply_force(tp_body *b, float fx, float fy) {
    if (!b) return;
    b->ax += fx * b->inv_mass;
    b->ay += fy * b->inv_mass;
}

static inline void tp_clear_forces(tp_body *b) {
    if (!b) return;
    b->ax = 0.0f;
    b->ay = 0.0f;
}

/* semi-implicit Euler integrator */
static inline void tp_integrate(tp_body *b, float dt) {
    if (!b || b->sleeping) return;
    b->vx += b->ax * dt;
    b->vy += b->ay * dt;
    /* damping */
    b->vx *= (1.0f - b->friction);
    b->vy *= (1.0f - b->friction);
    b->x += b->vx * dt;
    b->y += b->vy * dt;
    tp_clear_forces(b);
}

/* world management */
static inline void tp_world_init(tp_world *w, tp_body *array, int capacity, float dt) {
    if (!w) return;
    w->bodies = array;
    w->count = 0;
    w->capacity = (capacity > 0) ? capacity : 0;
    w->gravity_x = 0.0f;
    w->gravity_y = 980.0f;
    w->dt = dt;
}

static inline tp_body *tp_world_add_body(tp_world *w) {
    if (!w || w->count >= w->capacity) return NULL;
    tp_body *b = &w->bodies[w->count++];
    tp_init_body(b);
    return b;
}

/* --- small internal collision tests (circle-circle, aabb-aabb, circle-aabb) --- */

static inline float tp_absf(float v) { return v < 0.0f ? -v : v; }

static inline int tp_circle_circle_manifold(const tp_shape2d *A, const tp_shape2d *B, tp_manifold *m) {
    float dx = B->x - A->x;
    float dy = B->y - A->y;
    float r = A->r + B->r;
    float dist2 = dx*dx + dy*dy;
    if (dist2 >= r*r) return 0;
    float dist = sqrtf(dist2);
    if (dist > 1e-6f) {
        m->nx = dx / dist;
        m->ny = dy / dist;
    } else {
        m->nx = 1.0f; m->ny = 0.0f;
        dist = 0.0f;
    }
    m->depth = r - dist;
    m->px = A->x + m->nx * (A->r - m->depth*0.5f);
    m->py = A->y + m->ny * (A->r - m->depth*0.5f);
    return 1;
}

static inline int tp_aabb_aabb_manifold(const tp_shape2d *A, const tp_shape2d *B, tp_manifold *m) {
    float ahw = A->w * 0.5f, ahh = A->h * 0.5f;
    float bhw = B->w * 0.5f, bhh = B->h * 0.5f;
    float dx = B->x - A->x;
    float px = (ahw + bhw) - tp_absf(dx);
    if (px <= 0.0f) return 0;
    float dy = B->y - A->y;
    float py = (ahh + bhh) - tp_absf(dy);
    if (py <= 0.0f) return 0;
    if (px < py) {
        m->depth = px;
        m->nx = (dx < 0.0f) ? -1.0f : 1.0f;
        m->ny = 0.0f;
        m->px = A->x + m->nx * (ahw - m->depth*0.5f);
        m->py = B->y;
    } else {
        m->depth = py;
        m->nx = 0.0f;
        m->ny = (dy < 0.0f) ? -1.0f : 1.0f;
        m->px = B->x;
        m->py = A->y + m->ny * (ahh - m->depth*0.5f);
    }
    return 1;
}

// static inline int tp_circle_aabb_manifold(const tp_shape2d *C, const tp_shape2d *B, tp_manifold *m) {
//     float bxmin = B->x - B->w*0.5f;
//     float bymin = B->y - B->h*0.5f;
//     float closest_x = (C->x < bxmin) ? bxmin : ((C->x > bxmin + B->w) ? (bxmin + B->w) : C->x);
//     float closest_y = (C->y < bymin) ? bymin : ((C->y > bymin + B->h) ? (bymin + B->h) : C->y);
//     float dx = C->x - closest_x;
//     float dy = C->y - closest_y;
//     float dist2 = dx*dx + dy*dy;
//     if (dist2 >= (C->r * C->r)) return 0;
//     float dist = sqrtf(dist2);
//     if (dist > 1e-6f) {
//         m->nx = dx / dist;
//         m->ny = dy / dist;
//     } else {
//         /* choose normal toward box center */
//         float cx = B->x - C->x;
//         float cy = B->y - C->y;
//         float len = sqrtf(cx*cx + cy*cy);
//         if (len > 1e-6f) { m->nx = -cx/len; m->ny = -cy/len; }
//         else { m->nx = 1.0f; m->ny = 0.0f; }
//         dist = 0.0f;
//     }
//     m->depth = C->r - dist;
//     m->px = closest_x;
//     m->py = closest_y;
//     return 1;
// }
static inline int tp_circle_aabb_manifold(const tp_shape2d *C, const tp_shape2d *B, tp_manifold *m)
{
    float half_w = B->w * 0.5f;
    float half_h = B->h * 0.5f;
    float bx_min = B->x - half_w;
    float bx_max = B->x + half_w;
    float by_min = B->y - half_h;
    float by_max = B->y + half_h;

    // Closest point on AABB to circle center
    float closest_x = (C->x < bx_min) ? bx_min : (C->x > bx_max ? bx_max : C->x);
    float closest_y = (C->y < by_min) ? by_min : (C->y > by_max ? by_max : C->y);

    float dx = C->x - closest_x;
    float dy = C->y - closest_y;
    float dist2 = dx*dx + dy*dy;

    if (dist2 >= C->r * C->r) return 0;

    float dist = sqrtf(dist2);

    if (dist > 1e-6f)
    {
        m->nx = dx / dist;
        m->ny = dy / dist;
    }
    else
    {
        // Circle center inside box → push toward box center (rare but safe)
        float cx = B->x - C->x;
        float cy = B->y - C->y;
        float len = sqrtf(cx*cx + cy*cy);
        if (len > 1e-6f) { m->nx = -cx/len; m->ny = -cy/len; }
        else { m->nx = 1.0f; m->ny = 0.0f; }
        dist = 0.0f;
    }

    m->depth = C->r - dist;

    // CORRECT contact point: on the circle surface, along the normal
    m->px = C->x - m->nx * C->r;   // ← THIS IS THE FIX
    m->py = C->y - m->ny * C->r;   // ← THIS IS THE FIX

    return 1;
}


static inline int tp_compute_manifold(const tp_shape2d *A, const tp_shape2d *B, tp_manifold *m) {
    if (!A || !B || !m) return 0;
    if (A->shape == TP_SHAPE_CIRCLE && B->shape == TP_SHAPE_CIRCLE) {
        return tp_circle_circle_manifold(A, B, m);
    } else if (A->shape == TP_SHAPE_AABB && B->shape == TP_SHAPE_AABB) {
        return tp_aabb_aabb_manifold(A, B, m);
    } else if (A->shape == TP_SHAPE_CIRCLE && B->shape == TP_SHAPE_AABB) {
        return tp_circle_aabb_manifold(A, B, m);
    } else if (A->shape == TP_SHAPE_AABB && B->shape == TP_SHAPE_CIRCLE) {
        /* compute circle-aabb with swapped args then flip normal */
        tp_manifold tmp;
        int hit = tp_circle_aabb_manifold(B, A, &tmp);
        if (!hit) return 0;
        m->nx = -tmp.nx; m->ny = -tmp.ny;
        m->depth = tmp.depth;
        m->px = tmp.px; m->py = tmp.py;
        return 1;
    }
    return 0;
}

/* --- impulse resolution (basic) --- */
static inline void tp_apply_impulse(tp_body *A, tp_body *B, const tp_manifold *m) {
    if (!A || !B || !m) return;
    /* relative velocity */
    float rvx = B->vx - A->vx;
    float rvy = B->vy - A->vy;
    float relAlongNormal = rvx * m->nx + rvy * m->ny;
    if (relAlongNormal > 0.0f) return; /* separating */
    float e = (A->restitution < B->restitution) ? A->restitution : B->restitution;
    float j = -(1.0f + e) * relAlongNormal;
    float invMassSum = A->inv_mass + B->inv_mass;
    if (invMassSum <= 0.0f) return;
    j /= invMassSum;
    /* apply */
    if (A->inv_mass > 0.0f) {
        A->vx -= m->nx * j * A->inv_mass;
        A->vy -= m->ny * j * A->inv_mass;
    }
    if (B->inv_mass > 0.0f) {
        B->vx += m->nx * j * B->inv_mass;
        B->vy += m->ny * j * B->inv_mass;
    }
    /* simple friction tangent impulse (Coulomb-ish) */
    float tx = rvx - relAlongNormal * m->nx;
    float ty = rvy - relAlongNormal * m->ny;
    float tlen = sqrtf(tx*tx + ty*ty);
    if (tlen > 1e-6f) {
        tx /= tlen; ty /= tlen;
        float jt = -(rvx*tx + rvy*ty);
        jt /= invMassSum;
        /* clamp by friction (use min friction of two bodies) */
        float mu = (A->friction < B->friction) ? A->friction : B->friction;
        if (jt > j * mu) jt = j * mu;
        if (jt < -j * mu) jt = -j * mu;
        if (A->inv_mass > 0.0f) { A->vx -= tx * jt * A->inv_mass; A->vy -= ty * jt * A->inv_mass; }
        if (B->inv_mass > 0.0f) { B->vx += tx * jt * B->inv_mass; B->vy += ty * jt * B->inv_mass; }
    }
}

/* --- positional correction to remove sinking --- */
static inline void tp_positional_correction(tp_body *A, tp_body *B, const tp_manifold *m) {
    if (!A || !B || !m) return;
    const float percent = 0.8f; /* usually 20%..80% */
    const float slop = 0.01f;
    float correction = (m->depth > slop) ? (m->depth - slop) / (A->inv_mass + B->inv_mass) : 0.0f;
    if (correction <= 0.0f) return;
    float nx = m->nx, ny = m->ny;
    if (A->inv_mass > 0.0f) { A->x -= nx * correction * percent * A->inv_mass; A->y -= ny * correction * percent * A->inv_mass; }
    if (B->inv_mass > 0.0f) { B->x += nx * correction * percent * B->inv_mass; B->y += ny * correction * percent * B->inv_mass; }
}

/* --- world collision handling (naive O(n^2)) --- */
static inline void tp_world_handle_collisions(tp_world *w) {
    if (!w) return;
    for (int i = 0; i < w->count; ++i) {
        for (int j = i+1; j < w->count; ++j) {
            tp_body *A = &w->bodies[i];
            tp_body *B = &w->bodies[j];
            tp_shape2d SA = { A->x, A->y, A->w, A->h, A->radius, A->shape };
            tp_shape2d SB = { B->x, B->y, B->w, B->h, B->radius, B->shape };
            tp_manifold m;
            if (!tp_compute_manifold(&SA, &SB, &m)) continue;
            /* positional correction then impulse */
            tp_positional_correction(A, B, &m);
            tp_apply_impulse(A, B, &m);
        }
    }
}

/* --- single step: apply gravity, integrate, resolve collisions --- */
static inline void tp_world_step(tp_world *w) {
    if (!w) return;
    for (int i = 0; i < w->count; ++i) {
        tp_body *b = &w->bodies[i];
        if (b->inv_mass > 0.0f) {
            tp_apply_force(b, w->gravity_x * b->mass, w->gravity_y * b->mass);
        }
        tp_integrate(b, w->dt);
    }
    /* naive collision pass */
    tp_world_handle_collisions(w);
}

/* --- utility: convert body to shape descriptor if needed externally --- */
static inline tp_shape2d tp_body_to_shape(const tp_body *b) {
    tp_shape2d s;
    if (!b) { s.x = s.y = s.w = s.h = s.r = 0.0f; s.shape = TP_SHAPE_AABB; return s; }
    s.x = b->x; s.y = b->y; s.w = b->w; s.h = b->h; s.r = b->radius; s.shape = b->shape;
    return s;
}

#ifdef __cplusplus
}
#endif

#endif /* TINYPHYS_2D_H */

