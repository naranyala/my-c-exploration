#ifndef ORBIT_MATH_H
#define ORBIT_MATH_H

#include <math.h>
#include "raylib.h"   // Use Raylib's Vector3 directly

// --- Configurable Defaults ---
#define ORBIT_RADIUS 150.0f
#define ORBIT_SPEED 0.005f
#define INCLINATION_ANGLE (M_PI / 6.0)

// --- Types ---
typedef struct {
    float x_screen, y_screen, scale;
} Projected2D;

typedef struct {
    float centerX, centerY;
    float angle;
} OrbitState;

// --- Vector Utilities ---
static inline Vector3 vector_add(Vector3 a, Vector3 b) {
    return (Vector3){a.x + b.x, a.y + b.y, a.z + b.z};
}

static inline Vector3 vector_sub(Vector3 a, Vector3 b) {
    return (Vector3){a.x - b.x, a.y - b.y, a.z - b.z};
}

static inline Vector3 vector_scale(Vector3 v, float s) {
    return (Vector3){v.x * s, v.y * s, v.z * s};
}

static inline float vector_dot(Vector3 a, Vector3 b) {
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

static inline Vector3 vector_cross(Vector3 a, Vector3 b) {
    return (Vector3){
        a.y*b.z - a.z*b.y,
        a.z*b.x - a.x*b.z,
        a.x*b.y - a.y*b.x
    };
}

static inline float vector_length(Vector3 v) {
    return sqrtf(vector_dot(v, v));
}

static inline Vector3 vector_normalize(Vector3 v) {
    float len = vector_length(v);
    return len > 0 ? vector_scale(v, 1.0f/len) : v;
}

// --- Rotation Helpers ---
static inline Vector3 rotate_x(Vector3 v, float angle) {
    float c = cosf(angle), s = sinf(angle);
    return (Vector3){v.x, v.y*c - v.z*s, v.y*s + v.z*c};
}

static inline Vector3 rotate_y(Vector3 v, float angle) {
    float c = cosf(angle), s = sinf(angle);
    return (Vector3){v.x*c + v.z*s, v.y, -v.x*s + v.z*c};
}

static inline Vector3 rotate_z(Vector3 v, float angle) {
    float c = cosf(angle), s = sinf(angle);
    return (Vector3){v.x*c - v.y*s, v.x*s + v.y*c, v.z};
}

// --- Orbit Functions ---
static inline OrbitState init_orbit_state(float width, float height, float initial_angle) {
    return (OrbitState){width/2.0f, height/2.0f, initial_angle};
}

static inline Vector3 get_planet_position(const OrbitState* state, float radius, float angle) {
    Vector3 pos = {radius * cosf(angle), radius * sinf(angle), 0.0f};
    return rotate_x(pos, INCLINATION_ANGLE);
}

static inline Projected2D project_3d_to_2d(const OrbitState* state, Vector3 v, float canvas_depth) {
    float depth = canvas_depth + v.z;
    float scale = canvas_depth / depth;
    return (Projected2D){
        state->centerX + v.x * scale,
        state->centerY + v.y * scale,
        scale
    };
}

static inline void update_orbit_state(OrbitState* state, float delta_time) {
    state->angle += ORBIT_SPEED * delta_time;
}

#endif // ORBIT_MATH_H

