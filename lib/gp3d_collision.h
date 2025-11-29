/* gp3d_collision.h - General Purpose 3D Collision Detection
 *
 * Features:
 * - Point containment (AABB, Sphere, OBB)
 * - Primitive vs Primitive collision (AABB, Sphere, Ray, Triangle)
 * - Distance queries
 * - Ray casting (ray vs primitive)
 * - C99 compliant, header-only
 */

#ifndef GP3D_COLLISION_H
#define GP3D_COLLISION_H

#include "gp3d_math.h"

#ifndef GP3D_EPSILON_COLLISION
    #define GP3D_EPSILON_COLLISION 1e-6f
#endif

/* ========= Collision Primitives ========= */

// Axis-Aligned Bounding Box
typedef struct {
    gp3d_vec3 min;
    gp3d_vec3 max;
} gp3d_aabb;

// Oriented Bounding Box (center + half-extents + orientation)
typedef struct {
    gp3d_vec3 center;
    gp3d_vec3 half_extents; // Local-space half-sizes
    gp3d_mat4 transform;    // World matrix (columns = local axes + position)
} gp3d_obb;

// Sphere
typedef struct {
    gp3d_vec3 center;
    float radius;
} gp3d_sphere;

// Ray (origin + direction, direction should be normalized)
typedef struct {
    gp3d_vec3 origin;
    gp3d_vec3 direction; // Must be normalized!
} gp3d_ray;

// Triangle (3 points in world space)
typedef struct {
    gp3d_vec3 v0, v1, v2;
} gp3d_triangle;

/* ========= Utility Functions ========= */

// Create AABB from center and half-extents
static gp3d_aabb gp3d_aabb_from_center_extents(gp3d_vec3 center, gp3d_vec3 half_extents) {
    gp3d_aabb box;
    box.min = gp3d_vec3_sub(center, half_extents);
    box.max = gp3d_vec3_add(center, half_extents);
    return box;
}

// Create AABB from two points
static gp3d_aabb gp3d_aabb_from_points(gp3d_vec3 p1, gp3d_vec3 p2) {
    gp3d_aabb box;
    box.min = gp3d_vec3_make(
        fminf(p1.x, p2.x),
        fminf(p1.y, p2.y),
        fminf(p1.z, p2.z)
    );
    box.max = gp3d_vec3_make(
        fmaxf(p1.x, p2.x),
        fmaxf(p1.y, p2.y),
        fmaxf(p1.z, p2.z)
    );
    return box;
}

// Create OBB from center, half-extents, and orientation (yaw/pitch/roll or quat)
static gp3d_obb gp3d_obb_from_center_extents_quat(gp3d_vec3 center, gp3d_vec3 half_extents, gp3d_quat orientation) {
    gp3d_obb obb;
    obb.center = center;
    obb.half_extents = half_extents;
    obb.transform = gp3d_quat_to_mat4(orientation);
    // Set translation in matrix
    obb.transform.m[12] = center.x;
    obb.transform.m[13] = center.y;
    obb.transform.m[14] = center.z;
    return obb;
}

// Point inside AABB?
static int gp3d_aabb_contains_point(gp3d_aabb box, gp3d_vec3 p) {
    return (p.x >= box.min.x && p.x <= box.max.x &&
            p.y >= box.min.y && p.y <= box.max.y &&
            p.z >= box.min.z && p.z <= box.max.z);
}

// Point inside sphere?
static int gp3d_sphere_contains_point(gp3d_sphere s, gp3d_vec3 p) {
    float dist_sq = gp3d_vec3_dot(gp3d_vec3_sub(p, s.center), gp3d_vec3_sub(p, s.center));
    return dist_sq <= s.radius * s.radius;
}

// Sphere vs Sphere collision
static int gp3d_sphere_vs_sphere(gp3d_sphere a, gp3d_sphere b) {
    float dist_sq = gp3d_vec3_dot(gp3d_vec3_sub(a.center, b.center), gp3d_vec3_sub(a.center, b.center));
    float radius_sum = a.radius + b.radius;
    return dist_sq <= radius_sum * radius_sum; // ✅ Fixed
}

// AABB vs AABB collision
static int gp3d_aabb_vs_aabb(gp3d_aabb a, gp3d_aabb b) {
    return (a.min.x <= b.max.x && a.max.x >= b.min.x &&
            a.min.y <= b.max.y && a.max.y >= b.min.y &&
            a.min.z <= b.max.z && a.max.z >= b.min.z);
}

// Sphere vs AABB collision
static int gp3d_sphere_vs_aabb(gp3d_sphere s, gp3d_aabb box) {
    // Find closest point on AABB to sphere center
    gp3d_vec3 closest = gp3d_vec3_make(
        fmaxf(box.min.x, fminf(s.center.x, box.max.x)),
        fmaxf(box.min.y, fminf(s.center.y, box.max.y)),
        fmaxf(box.min.z, fminf(s.center.z, box.max.z))
    );
    float dist_sq = gp3d_vec3_dot(gp3d_vec3_sub(s.center, closest), gp3d_vec3_sub(s.center, closest));
    return dist_sq <= s.radius * s.radius;
}

// Ray vs Sphere intersection (returns hit distance, -1 if no hit)
static float gp3d_ray_vs_sphere(gp3d_ray ray, gp3d_sphere s) {
    gp3d_vec3 oc = gp3d_vec3_sub(ray.origin, s.center);
    float a = gp3d_vec3_dot(ray.direction, ray.direction);
    float b = 2.0f * gp3d_vec3_dot(oc, ray.direction);
    float c = gp3d_vec3_dot(oc, oc) - s.radius * s.radius;
    float discriminant = b * b - 4.0f * a * c;

    if (discriminant < 0) return -1.0f;

    float t = (-b - sqrtf(discriminant)) / (2.0f * a);
    if (t < 0) {
        t = (-b + sqrtf(discriminant)) / (2.0f * a);
    }
    return (t >= 0) ? t : -1.0f;
}

// Ray vs AABB intersection (slab method)
static float gp3d_ray_vs_aabb(gp3d_ray ray, gp3d_aabb box) {
    float tmin = (box.min.x - ray.origin.x) / ray.direction.x;
    float tmax = (box.max.x - ray.origin.x) / ray.direction.x;

    if (tmin > tmax) { float tmp = tmin; tmin = tmax; tmax = tmp; }

    float tymin = (box.min.y - ray.origin.y) / ray.direction.y;
    float tymax = (box.max.y - ray.origin.y) / ray.direction.y;

    if (tymin > tymax) { float tmp = tymin; tymin = tymax; tymax = tmp; }

    if ((tmin > tymax) || (tymin > tmax)) return -1.0f;

    if (tymin > tmin) tmin = tymin;
    if (tymax < tmax) tmax = tymax;

    float tzmin = (box.min.z - ray.origin.z) / ray.direction.z;
    float tzmax = (box.max.z - ray.origin.z) / ray.direction.z;

    if (tzmin > tzmax) { float tmp = tzmin; tzmin = tzmax; tzmax = tmp; }

    if ((tmin > tzmax) || (tzmin > tmax)) return -1.0f;

    if (tzmin > tmin) tmin = tzmin;
    if (tzmax < tmax) tmax = tzmax;

    return (tmin < 0) ? ((tmax > 0) ? tmax : -1.0f) : tmin;
}

// Point vs Triangle (barycentric method, returns distance squared if needed)
static int gp3d_point_vs_triangle(gp3d_vec3 p, gp3d_triangle tri, float* out_dist_sq) {
    gp3d_vec3 ab = gp3d_vec3_sub(tri.v1, tri.v0);
    gp3d_vec3 ac = gp3d_vec3_sub(tri.v2, tri.v0);
    gp3d_vec3 ap = gp3d_vec3_sub(p, tri.v0);

    float d1 = gp3d_vec3_dot(ab, ap);
    float d2 = gp3d_vec3_dot(ac, ap);
    if (d1 <= 0.0f && d2 <= 0.0f) {
        // Vertex region A
        if (out_dist_sq) *out_dist_sq = gp3d_vec3_dot(ap, ap);
        return (gp3d_vec3_dot(ap, ap) <= GP3D_EPSILON_COLLISION);
    }

    gp3d_vec3 bp = gp3d_vec3_sub(p, tri.v1);
    float d3 = gp3d_vec3_dot(ab, bp);
    float d4 = gp3d_vec3_dot(ac, bp);
    if (d3 >= 0.0f && d4 <= d3) {
        // Vertex region B
        if (out_dist_sq) *out_dist_sq = gp3d_vec3_dot(bp, bp);
        return (gp3d_vec3_dot(bp, bp) <= GP3D_EPSILON_COLLISION);
    }

    float vc = d1 * d4 - d3 * d2;
    if (vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f) {
        // Edge region AB
        float v = d1 / (d1 - d3);
        gp3d_vec3 proj = gp3d_vec3_add(tri.v0, gp3d_vec3_scale(ab, v));
        gp3d_vec3 diff = gp3d_vec3_sub(p, proj);
        if (out_dist_sq) *out_dist_sq = gp3d_vec3_dot(diff, diff);
        return (gp3d_vec3_dot(diff, diff) <= GP3D_EPSILON_COLLISION);
    }

    gp3d_vec3 cp = gp3d_vec3_sub(p, tri.v2);
    float d5 = gp3d_vec3_dot(ab, cp);
    float d6 = gp3d_vec3_dot(ac, cp);
    if (d6 >= 0.0f && d5 <= d6) {
        // Vertex region C
        if (out_dist_sq) *out_dist_sq = gp3d_vec3_dot(cp, cp);
        return (gp3d_vec3_dot(cp, cp) <= GP3D_EPSILON_COLLISION);
    }

    float vb = d5 * d2 - d1 * d6;
    if (vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f) {
        // Edge region AC
        float w = d2 / (d2 - d6);
        gp3d_vec3 proj = gp3d_vec3_add(tri.v0, gp3d_vec3_scale(ac, w));
        gp3d_vec3 diff = gp3d_vec3_sub(p, proj);
        if (out_dist_sq) *out_dist_sq = gp3d_vec3_dot(diff, diff);
        return (gp3d_vec3_dot(diff, diff) <= GP3D_EPSILON_COLLISION);
    }

    float va = d3 * d6 - d5 * d4;
    if (va <= 0.0f && (d4 - d3) >= 0.0f && (d5 - d6) >= 0.0f) {
        // Edge region BC
        float w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
        gp3d_vec3 edge = gp3d_vec3_sub(tri.v2, tri.v1);
        gp3d_vec3 proj = gp3d_vec3_add(tri.v1, gp3d_vec3_scale(edge, w));
        gp3d_vec3 diff = gp3d_vec3_sub(p, proj);
        if (out_dist_sq) *out_dist_sq = gp3d_vec3_dot(diff, diff);
        return (gp3d_vec3_dot(diff, diff) <= GP3D_EPSILON_COLLISION);
    }

    // Inside face region
    if (out_dist_sq) {
        gp3d_vec3 normal = gp3d_vec3_cross(ab, ac);
        normal = gp3d_vec3_normalize(normal);
        float dist = fabsf(gp3d_vec3_dot(normal, ap));
        *out_dist_sq = dist * dist;
    }
    return 1; // Inside triangle
}

#endif /* GP3D_COLLISION_H */
