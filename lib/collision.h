/* collision.h
 *
 * Standalone collision utilities.
 * - Uses col_ prefix for all public symbols.
 * - Provides shape types, intersection tests, manifold generation,
 *   raycast against shape, and a small broadphase helper.
 *
 * No dependency on any physics header.
 *
 * Usage:
 *   #include "collision.h"
 *   col_shape2d shapes[...];
 *   if (col_intersect_shape_manifold(&shapes[i], &shapes[j], &man)) { ... }
 */

#ifndef COLLISION_H
#define COLLISION_H

#include <math.h>
#include <string.h> /* memset */
#include <stdlib.h> /* NULL */

#ifdef __cplusplus
extern "C" {
#endif

/* --- shapes --- */
typedef enum {
    COL_SHAPE_CIRCLE = 0,
    COL_SHAPE_AABB   = 1
} col_shape_type;

typedef struct {
    float x, y;    /* center */
    float w, h;    /* full width/height for AABB */
    float r;       /* radius for circle */
    int shape;     /* col_shape_type */
} col_shape2d;

/* --- manifold --- */
typedef struct {
    float nx, ny;  /* normal from A -> B */
    float depth;   /* penetration */
    float px, py;  /* contact point */
} col_manifold;

/* --- helpers --- */
static inline float col_absf(float v) { return v < 0.0f ? -v : v; }
static inline float col_dotf(float ax, float ay, float bx, float by) { return ax*bx + ay*by; }

/* --- intersection implementations --- */

static inline int col_circle_circle(const col_shape2d *A, const col_shape2d *B, col_manifold *m) {
    float dx = B->x - A->x;
    float dy = B->y - A->y;
    float r = A->r + B->r;
    float dist2 = dx*dx + dy*dy;
    if (dist2 >= r*r) return 0;
    float dist = sqrtf(dist2);
    if (dist > 1e-6f) { m->nx = dx / dist; m->ny = dy / dist; }
    else { m->nx = 1.0f; m->ny = 0.0f; dist = 0.0f; }
    m->depth = r - dist;
    m->px = A->x + m->nx * (A->r - m->depth*0.5f);
    m->py = A->y + m->ny * (A->r - m->depth*0.5f);
    return 1;
}

static inline int col_aabb_aabb(const col_shape2d *A, const col_shape2d *B, col_manifold *m) {
    float ahw = A->w * 0.5f, ahh = A->h * 0.5f;
    float bhw = B->w * 0.5f, bhh = B->h * 0.5f;
    float dx = B->x - A->x;
    float px = (ahw + bhw) - col_absf(dx);
    if (px <= 0.0f) return 0;
    float dy = B->y - A->y;
    float py = (ahh + bhh) - col_absf(dy);
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

static inline int col_circle_aabb(const col_shape2d *C, const col_shape2d *B, col_manifold *m) {
    float bxmin = B->x - B->w*0.5f;
    float bymin = B->y - B->h*0.5f;
    float closest_x = (C->x < bxmin) ? bxmin : ((C->x > bxmin + B->w) ? (bxmin + B->w) : C->x);
    float closest_y = (C->y < bymin) ? bymin : ((C->y > bymin + B->h) ? (bymin + B->h) : C->y);
    float dx = C->x - closest_x;
    float dy = C->y - closest_y;
    float dist2 = dx*dx + dy*dy;
    if (dist2 >= (C->r * C->r)) return 0;
    float dist = sqrtf(dist2);
    if (dist > 1e-6f) { m->nx = dx / dist; m->ny = dy / dist; }
    else {
        float cx = B->x - C->x;
        float cy = B->y - C->y;
        float len = sqrtf(cx*cx + cy*cy);
        if (len > 1e-6f) { m->nx = -cx/len; m->ny = -cy/len; }
        else { m->nx = 1.0f; m->ny = 0.0f; }
        dist = 0.0f;
    }
    m->depth = C->r - dist;
    m->px = closest_x;
    m->py = closest_y;
    return 1;
}

/* public intersection API */
static inline int col_intersect_shape_manifold(const col_shape2d *A, const col_shape2d *B, col_manifold *m) {
    if (m) memset(m, 0, sizeof(*m));
    if (!A || !B) return 0;
    if (A->shape == COL_SHAPE_CIRCLE && B->shape == COL_SHAPE_CIRCLE) return col_circle_circle(A, B, m);
    if (A->shape == COL_SHAPE_AABB && B->shape == COL_SHAPE_AABB) return col_aabb_aabb(A, B, m);
    if (A->shape == COL_SHAPE_CIRCLE && B->shape == COL_SHAPE_AABB) return col_circle_aabb(A, B, m);
    if (A->shape == COL_SHAPE_AABB && B->shape == COL_SHAPE_CIRCLE) {
        col_manifold tmp;
        int hit = col_circle_aabb(B, A, &tmp);
        if (!hit) return 0;
        if (m) {
            m->nx = -tmp.nx; m->ny = -tmp.ny; m->depth = tmp.depth; m->px = tmp.px; m->py = tmp.py;
        }
        return 1;
    }
    return 0;
}

static inline int col_intersect_shape(const col_shape2d *A, const col_shape2d *B) {
    return col_intersect_shape_manifold(A, B, NULL);
}

/* --- raycast against shape (ray t in [0,1]) --- */
static inline int col_raycast_shape(const col_shape2d *s, float ox, float oy, float dx, float dy, float *out_t, float *out_x, float *out_y) {
    if (!s) return 0;
    if (s->shape == COL_SHAPE_CIRCLE) {
        float cx = s->x, cy = s->y;
        float r = s->r;
        float fx = ox - cx, fy = oy - cy;
        float a = dx*dx + dy*dy;
        float b = 2.0f * (fx*dx + fy*dy);
        float c = fx*fx + fy*fy - r*r;
        float disc = b*b - 4*a*c;
        if (disc < 0.0f) return 0;
        float sq = sqrtf(disc);
        float t0 = (-b - sq) / (2*a);
        float t1 = (-b + sq) / (2*a);
        float t = (t0 >= 0.0f && t0 <= 1.0f) ? t0 : ((t1 >= 0.0f && t1 <= 1.0f) ? t1 : -1.0f);
        if (t < 0.0f) return 0;
        if (out_t) *out_t = t;
        if (out_x) *out_x = ox + dx * t;
        if (out_y) *out_y = oy + dy * t;
        return 1;
    } else if (s->shape == COL_SHAPE_AABB) {
        float hx = s->w * 0.5f;
        float hy = s->h * 0.5f;
        float minx = s->x - hx, maxx = s->x + hx;
        float miny = s->y - hy, maxy = s->y + hy;
        float tmin = 0.0f, tmax = 1.0f;
        if (fabsf(dx) < 1e-9f) {
            if (ox < minx || ox > maxx) return 0;
        } else {
            float tx1 = (minx - ox) / dx, tx2 = (maxx - ox) / dx;
            if (tx1 > tx2) { float tmp = tx1; tx1 = tx2; tx2 = tmp; }
            if (tx1 > tmin) tmin = tx1;
            if (tx2 < tmax) tmax = tx2;
            if (tmin > tmax) return 0;
        }
        if (fabsf(dy) < 1e-9f) {
            if (oy < miny || oy > maxy) return 0;
        } else {
            float ty1 = (miny - oy) / dy, ty2 = (maxy - oy) / dy;
            if (ty1 > ty2) { float tmp = ty1; ty1 = ty2; ty2 = tmp; }
            if (ty1 > tmin) tmin = ty1;
            if (ty2 < tmax) tmax = ty2;
            if (tmin > tmax) return 0;
        }
        if (tmin < 0.0f || tmin > 1.0f) return 0;
        if (out_t) *out_t = tmin;
        if (out_x) *out_x = ox + dx * tmin;
        if (out_y) *out_y = oy + dy * tmin;
        return 1;
    }
    return 0;
}

/* --- simple broadphase: naive pair detector --- */
typedef void (*col_pair_callback)(int a_idx, int b_idx, void *user);

static inline void col_broadphase_bruteforce(const col_shape2d *shapes, int count, col_pair_callback cb, void *user) {
    if (!shapes || !cb) return;
    for (int i = 0; i < count; ++i) {
        for (int j = i+1; j < count; ++j) {
            if (col_intersect_shape(&shapes[i], &shapes[j])) cb(i, j, user);
        }
    }
}

#ifdef __cplusplus
}
#endif

#endif /* COLLISION_H */

