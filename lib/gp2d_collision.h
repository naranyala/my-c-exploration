// gp2d_collision.h - Collision Detection & Physics Helpers
// Depends on: gp2d_math.h for vector operations

#ifndef GP2D_COLLISION_H
#define GP2D_COLLISION_H

#include "gp2d_math.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ============================
 * Basic Shapes
 * ============================ */

// Axis-aligned bounding box
typedef struct { 
    gp2d_vec2 min; 
    gp2d_vec2 max; 
} gp2d_aabb;

// Circle
typedef struct { 
    gp2d_vec2 center; 
    float radius; 
} gp2d_circle;

// Line segment
typedef struct { 
    gp2d_vec2 a; 
    gp2d_vec2 b; 
} gp2d_segment;

// Ray
typedef struct { 
    gp2d_vec2 origin; 
    gp2d_vec2 direction; // Should be normalized
} gp2d_ray;

/* ============================
 * Hit/Collision Results
 * ============================ */

typedef struct {
    int hit;           // 1 if collision occurred
    gp2d_vec2 point;   // Point of collision
    gp2d_vec2 normal;  // Surface normal at collision
    float distance;    // Distance to collision
} gp2d_raycast_result;

typedef struct {
    int hit;           // 1 if overlapping
    gp2d_vec2 normal;  // Normalized direction (from A to B)
    float depth;       // Penetration depth
} gp2d_collision_result;

/* ============================
 * AABB Operations
 * ============================ */

static inline gp2d_aabb gp2d_aabb_make(gp2d_vec2 min, gp2d_vec2 max) {
    return (gp2d_aabb){min, max};
}

static inline gp2d_aabb gp2d_aabb_from_center(gp2d_vec2 center, gp2d_vec2 half_size) {
    return (gp2d_aabb){
        gp2d_v2_sub(center, half_size),
        gp2d_v2_add(center, half_size)
    };
}

static inline int gp2d_aabb_contains_point(gp2d_aabb box, gp2d_vec2 p) {
    return p.x >= box.min.x && p.x <= box.max.x &&
           p.y >= box.min.y && p.y <= box.max.y;
}

static inline int gp2d_aabb_overlap(gp2d_aabb a, gp2d_aabb b) {
    return (a.min.x <= b.max.x && a.max.x >= b.min.x &&
            a.min.y <= b.max.y && a.max.y >= b.min.y);
}

static inline gp2d_vec2 gp2d_aabb_center(gp2d_aabb box) {
    return gp2d_v2((box.min.x + box.max.x) * 0.5f,
                   (box.min.y + box.max.y) * 0.5f);
}

static inline gp2d_vec2 gp2d_aabb_size(gp2d_aabb box) {
    return gp2d_v2_sub(box.max, box.min);
}

/* ============================
 * Circle Collisions
 * ============================ */

static inline gp2d_circle gp2d_circle_make(gp2d_vec2 center, float radius) {
    return (gp2d_circle){center, radius};
}

static inline int gp2d_circle_contains_point(gp2d_circle c, gp2d_vec2 p) {
    return gp2d_v2_dist2(c.center, p) <= c.radius * c.radius;
}

static inline int gp2d_circle_overlap(gp2d_circle a, gp2d_circle b) {
    float rsum = a.radius + b.radius;
    return gp2d_v2_dist2(a.center, b.center) <= rsum * rsum;
}

static inline gp2d_collision_result gp2d_circle_collision(gp2d_circle a, gp2d_circle b) {
    gp2d_collision_result result = {0, gp2d_v2(0, 0), 0};
    
    gp2d_vec2 delta = gp2d_v2_sub(b.center, a.center);
    float dist2 = gp2d_v2_len2(delta);
    float rsum = a.radius + b.radius;
    
    if (dist2 <= rsum * rsum) {
        float dist = sqrtf(dist2);
        result.hit = 1;
        result.normal = (dist > 0) ? gp2d_v2_scale(delta, 1.0f / dist) : gp2d_v2(1, 0);
        result.depth = rsum - dist;
    }
    
    return result;
}

/* ============================
 * Circle vs AABB
 * ============================ */

static inline int gp2d_circle_aabb_overlap(gp2d_circle c, gp2d_aabb box) {
    // Find closest point on AABB to circle center
    float closest_x = fmaxf(box.min.x, fminf(c.center.x, box.max.x));
    float closest_y = fmaxf(box.min.y, fminf(c.center.y, box.max.y));
    gp2d_vec2 closest = gp2d_v2(closest_x, closest_y);
    
    return gp2d_v2_dist2(c.center, closest) <= c.radius * c.radius;
}

/* ============================
 * Ray Casting
 * ============================ */

static inline gp2d_ray gp2d_ray_make(gp2d_vec2 origin, gp2d_vec2 direction) {
    return (gp2d_ray){origin, gp2d_v2_norm(direction)};
}

// Ray vs Circle
static inline gp2d_raycast_result gp2d_raycast_circle(gp2d_ray ray, gp2d_circle c) {
    gp2d_raycast_result result = {0, gp2d_v2(0, 0), gp2d_v2(0, 0), 0};
    
    gp2d_vec2 oc = gp2d_v2_sub(ray.origin, c.center);
    float b = gp2d_v2_dot(oc, ray.direction);
    float c_val = gp2d_v2_dot(oc, oc) - c.radius * c.radius;
    float disc = b * b - c_val;
    
    if (disc < 0) return result;
    
    float t = -b - sqrtf(disc);
    if (t < 0) t = -b + sqrtf(disc);
    if (t < 0) return result;
    
    result.hit = 1;
    result.distance = t;
    result.point = gp2d_v2_add(ray.origin, gp2d_v2_scale(ray.direction, t));
    result.normal = gp2d_v2_norm(gp2d_v2_sub(result.point, c.center));
    
    return result;
}

// Ray vs AABB
static inline gp2d_raycast_result gp2d_raycast_aabb(gp2d_ray ray, gp2d_aabb box) {
    gp2d_raycast_result result = {0, gp2d_v2(0, 0), gp2d_v2(0, 0), 0};
    
    float tx1 = (box.min.x - ray.origin.x) / ray.direction.x;
    float tx2 = (box.max.x - ray.origin.x) / ray.direction.x;
    float tmin = fminf(tx1, tx2);
    float tmax = fmaxf(tx1, tx2);
    
    float ty1 = (box.min.y - ray.origin.y) / ray.direction.y;
    float ty2 = (box.max.y - ray.origin.y) / ray.direction.y;
    tmin = fmaxf(tmin, fminf(ty1, ty2));
    tmax = fminf(tmax, fmaxf(ty1, ty2));
    
    if (tmax < 0 || tmin > tmax) return result;
    
    float t = (tmin >= 0) ? tmin : tmax;
    result.hit = 1;
    result.distance = t;
    result.point = gp2d_v2_add(ray.origin, gp2d_v2_scale(ray.direction, t));
    
    // Calculate normal
    gp2d_vec2 center = gp2d_aabb_center(box);
    gp2d_vec2 to_point = gp2d_v2_sub(result.point, center);
    gp2d_vec2 half = gp2d_v2_scale(gp2d_aabb_size(box), 0.5f);
    
    float dx = fabsf(to_point.x / half.x);
    float dy = fabsf(to_point.y / half.y);
    
    if (dx > dy) {
        result.normal = gp2d_v2(to_point.x > 0 ? 1.0f : -1.0f, 0);
    } else {
        result.normal = gp2d_v2(0, to_point.y > 0 ? 1.0f : -1.0f);
    }
    
    return result;
}

/* ============================
 * Polygon Collision (SAT)
 * ============================ */

// Project polygon onto axis
static inline void gp2d_polygon_project(const gp2d_vec2 *verts, int count, 
                                        gp2d_vec2 axis, float *min_out, float *max_out) {
    float min = gp2d_v2_dot(verts[0], axis);
    float max = min;
    
    for (int i = 1; i < count; i++) {
        float proj = gp2d_v2_dot(verts[i], axis);
        if (proj < min) min = proj;
        if (proj > max) max = proj;
    }
    
    *min_out = min;
    *max_out = max;
}

// SAT overlap test for convex polygons
static inline int gp2d_polygon_overlap(const gp2d_vec2 *a, int na, 
                                       const gp2d_vec2 *b, int nb) {
    // Test edges of polygon A
    for (int i = 0; i < na; i++) {
        gp2d_vec2 edge = gp2d_v2_sub(a[(i + 1) % na], a[i]);
        gp2d_vec2 axis = gp2d_v2_norm(gp2d_v2_perp(edge));
        
        float minA, maxA, minB, maxB;
        gp2d_polygon_project(a, na, axis, &minA, &maxA);
        gp2d_polygon_project(b, nb, axis, &minB, &maxB);
        
        if (maxA < minB || maxB < minA) return 0;
    }
    
    // Test edges of polygon B
    for (int i = 0; i < nb; i++) {
        gp2d_vec2 edge = gp2d_v2_sub(b[(i + 1) % nb], b[i]);
        gp2d_vec2 axis = gp2d_v2_norm(gp2d_v2_perp(edge));
        
        float minA, maxA, minB, maxB;
        gp2d_polygon_project(a, na, axis, &minA, &maxA);
        gp2d_polygon_project(b, nb, axis, &minB, &maxB);
        
        if (maxA < minB || maxB < minA) return 0;
    }
    
    return 1; // All axes passed, shapes overlap
}

/* ============================
 * Physics Helpers
 * ============================ */

// Bounce velocity off surface
static inline gp2d_vec2 gp2d_reflect_velocity(gp2d_vec2 velocity, gp2d_vec2 normal, float bounce) {
    gp2d_vec2 reflected = gp2d_v2_reflect(velocity, normal);
    return gp2d_v2_scale(reflected, bounce);
}

// Slide velocity along surface (remove normal component)
static inline gp2d_vec2 gp2d_slide_velocity(gp2d_vec2 velocity, gp2d_vec2 normal) {
    float vn = gp2d_v2_dot(velocity, normal);
    return gp2d_v2_sub(velocity, gp2d_v2_scale(normal, vn));
}

// Separate two overlapping circles
static inline void gp2d_separate_circles(gp2d_circle *a, gp2d_circle *b) {
    gp2d_collision_result col = gp2d_circle_collision(*a, *b);
    if (col.hit) {
        float half_depth = col.depth * 0.5f;
        a->center = gp2d_v2_sub(a->center, gp2d_v2_scale(col.normal, half_depth));
        b->center = gp2d_v2_add(b->center, gp2d_v2_scale(col.normal, half_depth));
    }
}

#ifdef __cplusplus
}
#endif

#endif /* GP2D_COLLISION_H */
