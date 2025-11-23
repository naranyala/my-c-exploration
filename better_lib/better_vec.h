/*
 * better_vec.h - Single-header 2D/3D vector math
 * 
 * Pairs with better_math.h but works standalone too.
 * Define BETTER_VEC_NO_SHORT_NAMES to use bv_ prefix only.
 */

#ifndef BETTER_VEC_H
#define BETTER_VEC_H

#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef BV_PI
    #ifdef M_PI
        #define BV_PI M_PI
    #else
        #define BV_PI 3.14159265358979323846
    #endif
#endif

/* ══════════════════════════════════════════════════════════════════
   Vec2 - 2D Vector
   ══════════════════════════════════════════════════════════════════ */

typedef struct { float x, y; } Vec2;

/* Constructors */
static inline Vec2 bv_vec2(float x, float y) { return (Vec2){x, y}; }
static inline Vec2 bv_vec2_zero(void)  { return (Vec2){0, 0}; }
static inline Vec2 bv_vec2_one(void)   { return (Vec2){1, 1}; }
static inline Vec2 bv_vec2_up(void)    { return (Vec2){0, -1}; }
static inline Vec2 bv_vec2_down(void)  { return (Vec2){0, 1}; }
static inline Vec2 bv_vec2_left(void)  { return (Vec2){-1, 0}; }
static inline Vec2 bv_vec2_right(void) { return (Vec2){1, 0}; }

/* Arithmetic */
static inline Vec2 bv_vec2_add(Vec2 a, Vec2 b) { return (Vec2){a.x + b.x, a.y + b.y}; }
static inline Vec2 bv_vec2_sub(Vec2 a, Vec2 b) { return (Vec2){a.x - b.x, a.y - b.y}; }
static inline Vec2 bv_vec2_mul(Vec2 a, Vec2 b) { return (Vec2){a.x * b.x, a.y * b.y}; }
static inline Vec2 bv_vec2_div(Vec2 a, Vec2 b) { return (Vec2){a.x / b.x, a.y / b.y}; }
static inline Vec2 bv_vec2_scale(Vec2 v, float s) { return (Vec2){v.x * s, v.y * s}; }
static inline Vec2 bv_vec2_neg(Vec2 v) { return (Vec2){-v.x, -v.y}; }

/* Products */
static inline float bv_vec2_dot(Vec2 a, Vec2 b) { return a.x * b.x + a.y * b.y; }
static inline float bv_vec2_cross(Vec2 a, Vec2 b) { return a.x * b.y - a.y * b.x; }

/* Length */
static inline float bv_vec2_len_sq(Vec2 v) { return v.x * v.x + v.y * v.y; }
static inline float bv_vec2_len(Vec2 v) { return sqrtf(bv_vec2_len_sq(v)); }
static inline float bv_vec2_dist(Vec2 a, Vec2 b) { return bv_vec2_len(bv_vec2_sub(a, b)); }
static inline float bv_vec2_dist_sq(Vec2 a, Vec2 b) { return bv_vec2_len_sq(bv_vec2_sub(a, b)); }

/* Normalize */
static inline Vec2 bv_vec2_norm(Vec2 v) {
    float len = bv_vec2_len(v);
    if (len < 1e-8f) return bv_vec2_zero();
    return bv_vec2_scale(v, 1.0f / len);
}

/* Interpolation */
static inline Vec2 bv_vec2_lerp(Vec2 a, Vec2 b, float t) {
    return (Vec2){a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t};
}

/* Rotation */
static inline Vec2 bv_vec2_rotate(Vec2 v, float radians) {
    float c = cosf(radians), s = sinf(radians);
    return (Vec2){v.x * c - v.y * s, v.x * s + v.y * c};
}

static inline float bv_vec2_angle(Vec2 v) { return atan2f(v.y, v.x); }

static inline Vec2 bv_vec2_from_angle(float radians) {
    return (Vec2){cosf(radians), sinf(radians)};
}

/* Utilities */
static inline Vec2 bv_vec2_perpendicular(Vec2 v) { return (Vec2){-v.y, v.x}; }
static inline Vec2 bv_vec2_reflect(Vec2 v, Vec2 normal) {
    float d = 2.0f * bv_vec2_dot(v, normal);
    return bv_vec2_sub(v, bv_vec2_scale(normal, d));
}

static inline Vec2 bv_vec2_min(Vec2 a, Vec2 b) {
    return (Vec2){a.x < b.x ? a.x : b.x, a.y < b.y ? a.y : b.y};
}

static inline Vec2 bv_vec2_max(Vec2 a, Vec2 b) {
    return (Vec2){a.x > b.x ? a.x : b.x, a.y > b.y ? a.y : b.y};
}

static inline Vec2 bv_vec2_clamp(Vec2 v, Vec2 min, Vec2 max) {
    return bv_vec2_min(bv_vec2_max(v, min), max);
}

static inline Vec2 bv_vec2_abs(Vec2 v) {
    return (Vec2){fabsf(v.x), fabsf(v.y)};
}

static inline Vec2 bv_vec2_floor(Vec2 v) {
    return (Vec2){floorf(v.x), floorf(v.y)};
}

/* ══════════════════════════════════════════════════════════════════
   Vec3 - 3D Vector
   ══════════════════════════════════════════════════════════════════ */

typedef struct { float x, y, z; } Vec3;

/* Constructors */
static inline Vec3 bv_vec3(float x, float y, float z) { return (Vec3){x, y, z}; }
static inline Vec3 bv_vec3_zero(void)    { return (Vec3){0, 0, 0}; }
static inline Vec3 bv_vec3_one(void)     { return (Vec3){1, 1, 1}; }
static inline Vec3 bv_vec3_up(void)      { return (Vec3){0, 1, 0}; }
static inline Vec3 bv_vec3_down(void)    { return (Vec3){0, -1, 0}; }
static inline Vec3 bv_vec3_left(void)    { return (Vec3){-1, 0, 0}; }
static inline Vec3 bv_vec3_right(void)   { return (Vec3){1, 0, 0}; }
static inline Vec3 bv_vec3_forward(void) { return (Vec3){0, 0, -1}; }
static inline Vec3 bv_vec3_back(void)    { return (Vec3){0, 0, 1}; }

/* Arithmetic */
static inline Vec3 bv_vec3_add(Vec3 a, Vec3 b) { return (Vec3){a.x + b.x, a.y + b.y, a.z + b.z}; }
static inline Vec3 bv_vec3_sub(Vec3 a, Vec3 b) { return (Vec3){a.x - b.x, a.y - b.y, a.z - b.z}; }
static inline Vec3 bv_vec3_mul(Vec3 a, Vec3 b) { return (Vec3){a.x * b.x, a.y * b.y, a.z * b.z}; }
static inline Vec3 bv_vec3_div(Vec3 a, Vec3 b) { return (Vec3){a.x / b.x, a.y / b.y, a.z / b.z}; }
static inline Vec3 bv_vec3_scale(Vec3 v, float s) { return (Vec3){v.x * s, v.y * s, v.z * s}; }
static inline Vec3 bv_vec3_neg(Vec3 v) { return (Vec3){-v.x, -v.y, -v.z}; }

/* Products */
static inline float bv_vec3_dot(Vec3 a, Vec3 b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
static inline Vec3 bv_vec3_cross(Vec3 a, Vec3 b) {
    return (Vec3){
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

/* Length */
static inline float bv_vec3_len_sq(Vec3 v) { return v.x * v.x + v.y * v.y + v.z * v.z; }
static inline float bv_vec3_len(Vec3 v) { return sqrtf(bv_vec3_len_sq(v)); }
static inline float bv_vec3_dist(Vec3 a, Vec3 b) { return bv_vec3_len(bv_vec3_sub(a, b)); }
static inline float bv_vec3_dist_sq(Vec3 a, Vec3 b) { return bv_vec3_len_sq(bv_vec3_sub(a, b)); }

/* Normalize */
static inline Vec3 bv_vec3_norm(Vec3 v) {
    float len = bv_vec3_len(v);
    if (len < 1e-8f) return bv_vec3_zero();
    return bv_vec3_scale(v, 1.0f / len);
}

/* Interpolation */
static inline Vec3 bv_vec3_lerp(Vec3 a, Vec3 b, float t) {
    return (Vec3){a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t, a.z + (b.z - a.z) * t};
}

/* Utilities */
static inline Vec3 bv_vec3_reflect(Vec3 v, Vec3 normal) {
    float d = 2.0f * bv_vec3_dot(v, normal);
    return bv_vec3_sub(v, bv_vec3_scale(normal, d));
}

static inline Vec3 bv_vec3_min(Vec3 a, Vec3 b) {
    return (Vec3){
        a.x < b.x ? a.x : b.x,
        a.y < b.y ? a.y : b.y,
        a.z < b.z ? a.z : b.z
    };
}

static inline Vec3 bv_vec3_max(Vec3 a, Vec3 b) {
    return (Vec3){
        a.x > b.x ? a.x : b.x,
        a.y > b.y ? a.y : b.y,
        a.z > b.z ? a.z : b.z
    };
}

static inline Vec3 bv_vec3_clamp(Vec3 v, Vec3 min, Vec3 max) {
    return bv_vec3_min(bv_vec3_max(v, min), max);
}

static inline Vec3 bv_vec3_abs(Vec3 v) {
    return (Vec3){fabsf(v.x), fabsf(v.y), fabsf(v.z)};
}

/* ══════════════════════════════════════════════════════════════════
   Vec4 - 4D Vector / Homogeneous Coordinates / RGBA
   ══════════════════════════════════════════════════════════════════ */

typedef struct { float x, y, z, w; } Vec4;

static inline Vec4 bv_vec4(float x, float y, float z, float w) { return (Vec4){x, y, z, w}; }
static inline Vec4 bv_vec4_zero(void) { return (Vec4){0, 0, 0, 0}; }
static inline Vec4 bv_vec4_one(void)  { return (Vec4){1, 1, 1, 1}; }

static inline Vec4 bv_vec4_add(Vec4 a, Vec4 b) { return (Vec4){a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w}; }
static inline Vec4 bv_vec4_sub(Vec4 a, Vec4 b) { return (Vec4){a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w}; }
static inline Vec4 bv_vec4_scale(Vec4 v, float s) { return (Vec4){v.x * s, v.y * s, v.z * s, v.w * s}; }
static inline float bv_vec4_dot(Vec4 a, Vec4 b) { return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w; }

static inline Vec4 bv_vec4_lerp(Vec4 a, Vec4 b, float t) {
    return (Vec4){
        a.x + (b.x - a.x) * t,
        a.y + (b.y - a.y) * t,
        a.z + (b.z - a.z) * t,
        a.w + (b.w - a.w) * t
    };
}

/* Conversions */
static inline Vec3 bv_vec4_to_vec3(Vec4 v) { return (Vec3){v.x, v.y, v.z}; }
static inline Vec4 bv_vec3_to_vec4(Vec3 v, float w) { return (Vec4){v.x, v.y, v.z, w}; }
static inline Vec2 bv_vec3_to_vec2(Vec3 v) { return (Vec2){v.x, v.y}; }
static inline Vec3 bv_vec2_to_vec3(Vec2 v, float z) { return (Vec3){v.x, v.y, z}; }

/* ══════════════════════════════════════════════════════════════════
   Mat4 - 4x4 Matrix (column-major, OpenGL style)
   ══════════════════════════════════════════════════════════════════ */

typedef struct { float m[16]; } Mat4;

static inline Mat4 bv_mat4_identity(void) {
    return (Mat4){{
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    }};
}

static inline Mat4 bv_mat4_translate(Vec3 v) {
    Mat4 m = bv_mat4_identity();
    m.m[12] = v.x; m.m[13] = v.y; m.m[14] = v.z;
    return m;
}

static inline Mat4 bv_mat4_scale(Vec3 v) {
    Mat4 m = bv_mat4_identity();
    m.m[0] = v.x; m.m[5] = v.y; m.m[10] = v.z;
    return m;
}

static inline Mat4 bv_mat4_rotate_x(float radians) {
    float c = cosf(radians), s = sinf(radians);
    Mat4 m = bv_mat4_identity();
    m.m[5] = c;  m.m[6] = s;
    m.m[9] = -s; m.m[10] = c;
    return m;
}

static inline Mat4 bv_mat4_rotate_y(float radians) {
    float c = cosf(radians), s = sinf(radians);
    Mat4 m = bv_mat4_identity();
    m.m[0] = c;  m.m[2] = -s;
    m.m[8] = s;  m.m[10] = c;
    return m;
}

static inline Mat4 bv_mat4_rotate_z(float radians) {
    float c = cosf(radians), s = sinf(radians);
    Mat4 m = bv_mat4_identity();
    m.m[0] = c;  m.m[1] = s;
    m.m[4] = -s; m.m[5] = c;
    return m;
}

static inline Mat4 bv_mat4_mul(Mat4 a, Mat4 b) {
    Mat4 r = {0};
    for (int c = 0; c < 4; c++) {
        for (int row = 0; row < 4; row++) {
            r.m[c * 4 + row] =
                a.m[row]      * b.m[c * 4] +
                a.m[row + 4]  * b.m[c * 4 + 1] +
                a.m[row + 8]  * b.m[c * 4 + 2] +
                a.m[row + 12] * b.m[c * 4 + 3];
        }
    }
    return r;
}

static inline Vec4 bv_mat4_mul_vec4(Mat4 m, Vec4 v) {
    return (Vec4){
        m.m[0] * v.x + m.m[4] * v.y + m.m[8]  * v.z + m.m[12] * v.w,
        m.m[1] * v.x + m.m[5] * v.y + m.m[9]  * v.z + m.m[13] * v.w,
        m.m[2] * v.x + m.m[6] * v.y + m.m[10] * v.z + m.m[14] * v.w,
        m.m[3] * v.x + m.m[7] * v.y + m.m[11] * v.z + m.m[15] * v.w
    };
}

static inline Vec3 bv_mat4_mul_vec3(Mat4 m, Vec3 v) {
    Vec4 r = bv_mat4_mul_vec4(m, bv_vec3_to_vec4(v, 1.0f));
    return bv_vec4_to_vec3(r);
}

static inline Mat4 bv_mat4_perspective(float fov_rad, float aspect, float near, float far) {
    float f = 1.0f / tanf(fov_rad / 2.0f);
    Mat4 m = {0};
    m.m[0]  = f / aspect;
    m.m[5]  = f;
    m.m[10] = (far + near) / (near - far);
    m.m[11] = -1.0f;
    m.m[14] = (2.0f * far * near) / (near - far);
    return m;
}

static inline Mat4 bv_mat4_ortho(float l, float r, float b, float t, float n, float f) {
    Mat4 m = bv_mat4_identity();
    m.m[0]  = 2.0f / (r - l);
    m.m[5]  = 2.0f / (t - b);
    m.m[10] = -2.0f / (f - n);
    m.m[12] = -(r + l) / (r - l);
    m.m[13] = -(t + b) / (t - b);
    m.m[14] = -(f + n) / (f - n);
    return m;
}

static inline Mat4 bv_mat4_look_at(Vec3 eye, Vec3 target, Vec3 up) {
    Vec3 f = bv_vec3_norm(bv_vec3_sub(target, eye));
    Vec3 r = bv_vec3_norm(bv_vec3_cross(f, up));
    Vec3 u = bv_vec3_cross(r, f);
    Mat4 m = bv_mat4_identity();
    m.m[0] = r.x;  m.m[4] = r.y;  m.m[8]  = r.z;
    m.m[1] = u.x;  m.m[5] = u.y;  m.m[9]  = u.z;
    m.m[2] = -f.x; m.m[6] = -f.y; m.m[10] = -f.z;
    m.m[12] = -bv_vec3_dot(r, eye);
    m.m[13] = -bv_vec3_dot(u, eye);
    m.m[14] = bv_vec3_dot(f, eye);
    return m;
}

/* ══════════════════════════════════════════════════════════════════
   Short Names
   ══════════════════════════════════════════════════════════════════ */

#ifndef BETTER_VEC_NO_SHORT_NAMES
    /* Vec2 */
    #define vec2            bv_vec2
    #define vec2_zero       bv_vec2_zero
    #define vec2_one        bv_vec2_one
    #define vec2_add        bv_vec2_add
    #define vec2_sub        bv_vec2_sub
    #define vec2_mul        bv_vec2_mul
    #define vec2_div        bv_vec2_div
    #define vec2_scale      bv_vec2_scale
    #define vec2_neg        bv_vec2_neg
    #define vec2_dot        bv_vec2_dot
    #define vec2_cross      bv_vec2_cross
    #define vec2_len        bv_vec2_len
    #define vec2_len_sq     bv_vec2_len_sq
    #define vec2_dist       bv_vec2_dist
    #define vec2_dist_sq    bv_vec2_dist_sq
    #define vec2_norm       bv_vec2_norm
    #define vec2_lerp       bv_vec2_lerp
    #define vec2_rotate     bv_vec2_rotate
    #define vec2_angle      bv_vec2_angle
    #define vec2_from_angle bv_vec2_from_angle
    #define vec2_perpendicular bv_vec2_perpendicular
    #define vec2_reflect    bv_vec2_reflect
    #define vec2_min        bv_vec2_min
    #define vec2_max        bv_vec2_max
    #define vec2_clamp      bv_vec2_clamp
    #define vec2_abs        bv_vec2_abs
    #define vec2_floor      bv_vec2_floor

    /* Vec3 */
    #define vec3            bv_vec3
    #define vec3_zero       bv_vec3_zero
    #define vec3_one        bv_vec3_one
    #define vec3_up         bv_vec3_up
    #define vec3_forward    bv_vec3_forward
    #define vec3_right      bv_vec3_right
    #define vec3_add        bv_vec3_add
    #define vec3_sub        bv_vec3_sub
    #define vec3_mul        bv_vec3_mul
    #define vec3_div        bv_vec3_div
    #define vec3_scale      bv_vec3_scale
    #define vec3_neg        bv_vec3_neg
    #define vec3_dot        bv_vec3_dot
    #define vec3_cross      bv_vec3_cross
    #define vec3_len        bv_vec3_len
    #define vec3_len_sq     bv_vec3_len_sq
    #define vec3_dist       bv_vec3_dist
    #define vec3_dist_sq    bv_vec3_dist_sq
    #define vec3_norm       bv_vec3_norm
    #define vec3_lerp       bv_vec3_lerp
    #define vec3_reflect    bv_vec3_reflect
    #define vec3_min        bv_vec3_min
    #define vec3_max        bv_vec3_max
    #define vec3_clamp      bv_vec3_clamp
    #define vec3_abs        bv_vec3_abs

    /* Vec4 */
    #define vec4            bv_vec4
    #define vec4_zero       bv_vec4_zero
    #define vec4_one        bv_vec4_one
    #define vec4_add        bv_vec4_add
    #define vec4_sub        bv_vec4_sub
    #define vec4_scale      bv_vec4_scale
    #define vec4_dot        bv_vec4_dot
    #define vec4_lerp       bv_vec4_lerp

    /* Conversions */
    #define vec4_to_vec3    bv_vec4_to_vec3
    #define vec3_to_vec4    bv_vec3_to_vec4
    #define vec3_to_vec2    bv_vec3_to_vec2
    #define vec2_to_vec3    bv_vec2_to_vec3

    /* Mat4 */
    #define mat4_identity   bv_mat4_identity
    #define mat4_translate  bv_mat4_translate
    #define mat4_scale      bv_mat4_scale
    #define mat4_rotate_x   bv_mat4_rotate_x
    #define mat4_rotate_y   bv_mat4_rotate_y
    #define mat4_rotate_z   bv_mat4_rotate_z
    #define mat4_mul        bv_mat4_mul
    #define mat4_mul_vec4   bv_mat4_mul_vec4
    #define mat4_mul_vec3   bv_mat4_mul_vec3
    #define mat4_perspective bv_mat4_perspective
    #define mat4_ortho      bv_mat4_ortho
    #define mat4_look_at    bv_mat4_look_at
#endif

#ifdef __cplusplus
}
#endif

#endif /* BETTER_VEC_H */
