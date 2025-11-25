/*
 * collision.h - Single-header 2D/3D collision detection library for C (C99)
 *
 * Features:
 *   2D:
 *     - AABB2 vs AABB2
 *     - Circle vs Circle
 *     - AABB2 vs Circle
 *   3D:
 *     - AABB3 vs AABB3
 *     - Sphere vs Sphere
 *     - AABB3 vs Sphere
 *
 * Usage:
 *   #define COLLISION_IMPLEMENTATION
 *   #include "collision.h"
 *
 * Author: Qwen AI
 * Date: 2025-11-25
 */

#ifndef COLLISION_H
#define COLLISION_H

#ifdef __cplusplus
extern "C" {
#endif

// --- 2D Types ---
typedef struct { float x, y; } Vec2;
typedef struct { Vec2 min, max; } AABB2;
typedef struct { Vec2 center; float radius; } Circle;

// --- 3D Types ---
typedef struct { float x, y, z; } Vec3;
typedef struct { Vec3 min, max; } AABB3;
typedef struct { Vec3 center; float radius; } Sphere;

// --- 2D Collision Functions ---
int collide_aabb2_aabb2(const AABB2* a, const AABB2* b);
int collide_circle_circle(const Circle* a, const Circle* b);
int collide_aabb2_circle(const AABB2* box, const Circle* circle);

// --- 3D Collision Functions ---
int collide_aabb3_aabb3(const AABB3* a, const AABB3* b);
int collide_sphere_sphere(const Sphere* a, const Sphere* b);
int collide_aabb3_sphere(const AABB3* box, const Sphere* sphere);

#ifdef __cplusplus
}
#endif

#endif // COLLISION_H

// ==============================
// IMPLEMENTATION
// ==============================
#ifdef COLLISION_IMPLEMENTATION

#include <math.h>

// --- Helper: clamp ---
static inline float clampf(float v, float lo, float hi) {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

// ============== 2D ==============

int collide_aabb2_aabb2(const AABB2* a, const AABB2* b) {
    if (a->max.x < b->min.x || b->max.x < a->min.x) return 0;
    if (a->max.y < b->min.y || b->max.y < a->min.y) return 0;
    return 1;
}

int collide_circle_circle(const Circle* a, const Circle* b) {
    float dx = b->center.x - a->center.x;
    float dy = b->center.y - a->center.y;
    float dist_sq = dx*dx + dy*dy;
    float r = a->radius + b->radius;
    return dist_sq <= r*r;
}

int collide_aabb2_circle(const AABB2* box, const Circle* circle) {
    float cx = clampf(circle->center.x, box->min.x, box->max.x);
    float cy = clampf(circle->center.y, box->min.y, box->max.y);
    float dx = circle->center.x - cx;
    float dy = circle->center.y - cy;
    float dist_sq = dx*dx + dy*dy;
    return dist_sq <= circle->radius * circle->radius;
}

// ============== 3D ==============

int collide_aabb3_aabb3(const AABB3* a, const AABB3* b) {
    if (a->max.x < b->min.x || b->max.x < a->min.x) return 0;
    if (a->max.y < b->min.y || b->max.y < a->min.y) return 0;
    if (a->max.z < b->min.z || b->max.z < a->min.z) return 0;
    return 1;
}

int collide_sphere_sphere(const Sphere* a, const Sphere* b) {
    float dx = b->center.x - a->center.x;
    float dy = b->center.y - a->center.y;
    float dz = b->center.z - a->center.z;
    float dist_sq = dx*dx + dy*dy + dz*dz;
    float r = a->radius + b->radius;
    return dist_sq <= r*r;
}

int collide_aabb3_sphere(const AABB3* box, const Sphere* sphere) {
    float cx = clampf(sphere->center.x, box->min.x, box->max.x);
    float cy = clampf(sphere->center.y, box->min.y, box->max.y);
    float cz = clampf(sphere->center.z, box->min.z, box->max.z);
    float dx = sphere->center.x - cx;
    float dy = sphere->center.y - cy;
    float dz = sphere->center.z - cz;
    float dist_sq = dx*dx + dy*dy + dz*dz;
    return dist_sq <= sphere->radius * sphere->radius;
}

#endif // COLLISION_IMPLEMENTATION
