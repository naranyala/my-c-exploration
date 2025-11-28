// gp2d_math.h - Core 2D Math Library (NO collision detection)
// Pure math operations: vectors, matrices, curves, animation, easing

#ifndef GP2D_MATH_H
#define GP2D_MATH_H

#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ============================
 * 2D Vector - Core Operations
 * ============================ */
typedef struct { float x, y; } gp2d_vec2;

// Construction & arithmetic
static inline gp2d_vec2 gp2d_v2(float x, float y) { 
    return (gp2d_vec2){x, y}; 
}
static inline gp2d_vec2 gp2d_v2_add(gp2d_vec2 a, gp2d_vec2 b) { 
    return gp2d_v2(a.x + b.x, a.y + b.y); 
}
static inline gp2d_vec2 gp2d_v2_sub(gp2d_vec2 a, gp2d_vec2 b) { 
    return gp2d_v2(a.x - b.x, a.y - b.y); 
}
static inline gp2d_vec2 gp2d_v2_scale(gp2d_vec2 a, float s) { 
    return gp2d_v2(a.x * s, a.y * s); 
}
static inline gp2d_vec2 gp2d_v2_mul(gp2d_vec2 a, gp2d_vec2 b) { 
    return gp2d_v2(a.x * b.x, a.y * b.y); 
}
static inline gp2d_vec2 gp2d_v2_neg(gp2d_vec2 a) { 
    return gp2d_v2(-a.x, -a.y); 
}

// Dot product & length
static inline float gp2d_v2_dot(gp2d_vec2 a, gp2d_vec2 b) { 
    return a.x * b.x + a.y * b.y; 
}
static inline float gp2d_v2_len2(gp2d_vec2 a) { 
    return gp2d_v2_dot(a, a); 
}
static inline float gp2d_v2_len(gp2d_vec2 a) { 
    return sqrtf(gp2d_v2_len2(a)); 
}
static inline float gp2d_v2_dist2(gp2d_vec2 a, gp2d_vec2 b) { 
    return gp2d_v2_len2(gp2d_v2_sub(b, a)); 
}
static inline float gp2d_v2_dist(gp2d_vec2 a, gp2d_vec2 b) { 
    return sqrtf(gp2d_v2_dist2(a, b)); 
}

// Normalization
static inline gp2d_vec2 gp2d_v2_norm(gp2d_vec2 a) {
    float m = gp2d_v2_len(a);
    return m > 0.0f ? gp2d_v2_scale(a, 1.0f / m) : a;
}

// Perpendicular (90° rotation)
static inline gp2d_vec2 gp2d_v2_perp(gp2d_vec2 a) { 
    return gp2d_v2(-a.y, a.x); 
}

// Interpolation
static inline gp2d_vec2 gp2d_v2_lerp(gp2d_vec2 a, gp2d_vec2 b, float t) {
    return gp2d_v2(a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t);
}

// Rotation
static inline float gp2d_v2_angle(gp2d_vec2 a) { 
    return atan2f(a.y, a.x); 
}
static inline gp2d_vec2 gp2d_v2_from_angle(float rad) { 
    return gp2d_v2(cosf(rad), sinf(rad)); 
}
static inline gp2d_vec2 gp2d_v2_rotate(gp2d_vec2 v, float rad) {
    float c = cosf(rad), s = sinf(rad);
    return gp2d_v2(v.x * c - v.y * s, v.x * s + v.y * c);
}

// Reflection
static inline gp2d_vec2 gp2d_v2_reflect(gp2d_vec2 v, gp2d_vec2 normal) {
    return gp2d_v2_sub(v, gp2d_v2_scale(normal, 2.0f * gp2d_v2_dot(v, normal)));
}

/* ============================
 * Bezier Curves
 * ============================ */

// Quadratic bezier
static inline gp2d_vec2 gp2d_v2_bezier_quad(gp2d_vec2 p0, gp2d_vec2 p1, gp2d_vec2 p2, float t) {
    gp2d_vec2 a = gp2d_v2_lerp(p0, p1, t);
    gp2d_vec2 b = gp2d_v2_lerp(p1, p2, t);
    return gp2d_v2_lerp(a, b, t);
}

// Cubic bezier
static inline gp2d_vec2 gp2d_v2_bezier_cubic(gp2d_vec2 p0, gp2d_vec2 p1, gp2d_vec2 p2, gp2d_vec2 p3, float t) {
    float u = 1 - t;
    float tt = t * t, uu = u * u;
    float uuu = uu * u, ttt = tt * t;
    gp2d_vec2 p = gp2d_v2_scale(p0, uuu);
    p = gp2d_v2_add(p, gp2d_v2_scale(p1, 3 * uu * t));
    p = gp2d_v2_add(p, gp2d_v2_scale(p2, 3 * u * tt));
    p = gp2d_v2_add(p, gp2d_v2_scale(p3, ttt));
    return p;
}

/* ============================
 * 3x3 Matrix (2D Transforms)
 * ============================ */
typedef struct { float m[9]; } gp2d_mat3;

static inline gp2d_mat3 gp2d_m3_identity(void) {
    gp2d_mat3 M = {0};
    M.m[0] = M.m[4] = M.m[8] = 1.0f;
    return M;
}

static inline gp2d_mat3 gp2d_m3_mul(gp2d_mat3 A, gp2d_mat3 B) {
    gp2d_mat3 R = {0};
    for (int c = 0; c < 3; c++)
        for (int r = 0; r < 3; r++)
            for (int k = 0; k < 3; k++)
                R.m[c * 3 + r] += A.m[k * 3 + r] * B.m[c * 3 + k];
    return R;
}

static inline gp2d_mat3 gp2d_m3_translate(float tx, float ty) {
    gp2d_mat3 T = gp2d_m3_identity();
    T.m[6] = tx; 
    T.m[7] = ty;
    return T;
}

static inline gp2d_mat3 gp2d_m3_scale(float sx, float sy) {
    gp2d_mat3 S = gp2d_m3_identity();
    S.m[0] = sx; 
    S.m[4] = sy;
    return S;
}

static inline gp2d_mat3 gp2d_m3_rotate(float angle) {
    float c = cosf(angle), s = sinf(angle);
    gp2d_mat3 R = gp2d_m3_identity();
    R.m[0] = c;  R.m[3] = -s;
    R.m[1] = s;  R.m[4] = c;
    return R;
}

// Transform application
static inline gp2d_vec2 gp2d_m3_transform_point(gp2d_mat3 M, gp2d_vec2 p) {
    float x = M.m[0] * p.x + M.m[3] * p.y + M.m[6];
    float y = M.m[1] * p.x + M.m[4] * p.y + M.m[7];
    return gp2d_v2(x, y);
}

static inline gp2d_vec2 gp2d_m3_transform_vec(gp2d_mat3 M, gp2d_vec2 v) {
    float x = M.m[0] * v.x + M.m[3] * v.y;
    float y = M.m[1] * v.x + M.m[4] * v.y;
    return gp2d_v2(x, y);
}

/* ============================
 * Easing Functions
 * ============================ */

static inline float gp2d_ease_linear(float t) { return t; }

// Quadratic
static inline float gp2d_ease_in_quad(float t) { return t * t; }
static inline float gp2d_ease_out_quad(float t) { return t * (2 - t); }
static inline float gp2d_ease_in_out_quad(float t) { 
    return t < 0.5f ? 2 * t * t : -1 + (4 - 2 * t) * t; 
}

// Cubic
static inline float gp2d_ease_in_cubic(float t) { return t * t * t; }
static inline float gp2d_ease_out_cubic(float t) { 
    float s = t - 1; 
    return s * s * s + 1; 
}
static inline float gp2d_ease_in_out_cubic(float t) {
    return t < 0.5f ? 4 * t * t * t : (t - 1) * (2 * t - 2) * (2 * t - 2) + 1;
}

// Exponential
static inline float gp2d_ease_in_expo(float t) { 
    return t == 0 ? 0 : powf(2, 10 * (t - 1)); 
}
static inline float gp2d_ease_out_expo(float t) { 
    return t == 1 ? 1 : 1 - powf(2, -10 * t); 
}

// Elastic
static inline float gp2d_ease_out_elastic(float t) {
    if (t == 0 || t == 1) return t;
    return powf(2, -10 * t) * sinf((t - 0.075f) * (2 * 3.14159f) / 0.3f) + 1;
}


/* ============================
 * Timing & Animation
 * ============================ */

// Time normalization
static inline float gp2d_time_normalize(float t, float start, float end) {
    return (t - start) / (end - start);
}

static inline float gp2d_clamp(float v, float min, float max) {
    return v < min ? min : (v > max ? max : v);
}

static inline float gp2d_clamp01(float v) {
    return gp2d_clamp(v, 0.0f, 1.0f);
}

// Simple timer
typedef struct { 
    float start; 
    float duration; 
    int loop; 
} gp2d_timer;

static inline gp2d_timer gp2d_timer_make(float duration, int loop) {
    return (gp2d_timer){.start = 0, .duration = duration, .loop = loop};
}

static inline float gp2d_timer_progress(gp2d_timer *t, float global_time) {
    float local = global_time - t->start;
    if (t->duration > 0 && t->loop) local = fmodf(local, t->duration);
    return (t->duration > 0) ? local / t->duration : 0;
}

static inline int gp2d_timer_done(gp2d_timer *t, float global_time) {
    return !t->loop && (global_time - t->start >= t->duration);
}

static inline void gp2d_timer_restart(gp2d_timer *t, float now) { 
    t->start = now; 
}

/* ============================
 * Keyframe Animation
 * ============================ */

typedef struct { float r, g, b, a; } gp2d_color;

typedef struct { float time; float value; } gp2d_key_float;
typedef struct { float time; gp2d_vec2 value; } gp2d_key_vec2;
typedef struct { float time; gp2d_color value; } gp2d_key_color;

static inline float gp2d_sample_float(const gp2d_key_float *keys, int count, float t, int loop) {
    if (count == 0) return 0;
    
    if (loop && count > 1) {
        float total_duration = keys[count - 1].time - keys[0].time;
        if (total_duration > 0) {
            t = fmodf(t - keys[0].time, total_duration) + keys[0].time;
        }
    }
    
    if (count == 1 || t <= keys[0].time) return keys[0].value;
    if (t >= keys[count - 1].time) return keys[count - 1].value;

    for (int i = 1; i < count; i++) {
        if (t < keys[i].time) {
            float a = keys[i - 1].time, b = keys[i].time;
            float p = (t - a) / (b - a);
            p = gp2d_ease_in_out_cubic(p);
            return keys[i - 1].value + (keys[i].value - keys[i - 1].value) * p;
        }
    }
    return keys[count - 1].value;
}

static inline gp2d_vec2 gp2d_sample_vec2(const gp2d_key_vec2 *keys, int count, float t, int loop) {
    if (count == 0) return gp2d_v2(0, 0);
    
    if (loop && count > 1) {
        float total_duration = keys[count - 1].time - keys[0].time;
        if (total_duration > 0) {
            t = fmodf(t - keys[0].time, total_duration) + keys[0].time;
        }
    }
    
    if (count == 1 || t <= keys[0].time) return keys[0].value;
    if (t >= keys[count - 1].time) return keys[count - 1].value;

    for (int i = 1; i < count; i++) {
        if (t < keys[i].time) {
            float a = keys[i - 1].time, b = keys[i].time;
            float p = gp2d_ease_in_out_cubic((t - a) / (b - a));
            return gp2d_v2_lerp(keys[i - 1].value, keys[i].value, p);
        }
    }
    return keys[count - 1].value;
}

/* ============================
 * Path Following (Bezier Chains)
 * ============================ */

typedef struct {
    gp2d_vec2 points[4];  // p0, p1, p2, p3
} gp2d_bezier_segment;

static inline gp2d_vec2 gp2d_path_sample(const gp2d_bezier_segment *segments, int count, float t) {
    if (count == 0) return gp2d_v2(0, 0);
    float segment_t = t * count;
    int idx = (int)segment_t;
    if (idx >= count) idx = count - 1;
    float local_t = segment_t - idx;

    const gp2d_bezier_segment *s = &segments[idx];
    return gp2d_v2_bezier_cubic(s->points[0], s->points[1], s->points[2], s->points[3], local_t);
}

#ifdef __cplusplus
}
#endif

#endif /* GP2D_MATH_H */
