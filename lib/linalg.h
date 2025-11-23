#ifndef LINALG_H
#define LINALG_H

#include <math.h>
#include <stdbool.h>

#define LINALG_EPSILON 1e-6f

// ============================================================================
// Vec3
// ============================================================================

typedef struct { float x, y, z; } Vec3;

// Construction
static inline Vec3 vec3(float x, float y, float z) {
    return (Vec3){x, y, z};
}

static inline Vec3 vec3_zero(void) {
    return (Vec3){0.0f, 0.0f, 0.0f};
}

static inline Vec3 vec3_one(void) {
    return (Vec3){1.0f, 1.0f, 1.0f};
}

// Basic arithmetic
static inline Vec3 vec3_add(Vec3 a, Vec3 b) {
    return (Vec3){a.x + b.x, a.y + b.y, a.z + b.z};
}

static inline Vec3 vec3_sub(Vec3 a, Vec3 b) {
    return (Vec3){a.x - b.x, a.y - b.y, a.z - b.z};
}

static inline Vec3 vec3_mul(Vec3 a, float s) {
    return (Vec3){a.x * s, a.y * s, a.z * s};
}

static inline Vec3 vec3_div(Vec3 a, float s) {
    float inv = 1.0f / s;
    return (Vec3){a.x * inv, a.y * inv, a.z * inv};
}

static inline Vec3 vec3_neg(Vec3 a) {
    return (Vec3){-a.x, -a.y, -a.z};
}

// Component-wise operations
static inline Vec3 vec3_mul_vec(Vec3 a, Vec3 b) {
    return (Vec3){a.x * b.x, a.y * b.y, a.z * b.z};
}

static inline Vec3 vec3_min(Vec3 a, Vec3 b) {
    return (Vec3){fminf(a.x, b.x), fminf(a.y, b.y), fminf(a.z, b.z)};
}

static inline Vec3 vec3_max(Vec3 a, Vec3 b) {
    return (Vec3){fmaxf(a.x, b.x), fmaxf(a.y, b.y), fmaxf(a.z, b.z)};
}

// Vector operations
static inline float vec3_dot(Vec3 a, Vec3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

static inline Vec3 vec3_cross(Vec3 a, Vec3 b) {
    return (Vec3){
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

static inline float vec3_len_sq(Vec3 a) {
    return vec3_dot(a, a);
}

static inline float vec3_len(Vec3 a) {
    return sqrtf(vec3_len_sq(a));
}

static inline float vec3_dist_sq(Vec3 a, Vec3 b) {
    return vec3_len_sq(vec3_sub(a, b));
}

static inline float vec3_dist(Vec3 a, Vec3 b) {
    return vec3_len(vec3_sub(a, b));
}

static inline Vec3 vec3_normalize(Vec3 a) {
    float len = vec3_len(a);
    return len > LINALG_EPSILON ? vec3_div(a, len) : vec3_zero();
}

static inline Vec3 vec3_lerp(Vec3 a, Vec3 b, float t) {
    return vec3_add(vec3_mul(a, 1.0f - t), vec3_mul(b, t));
}

static inline Vec3 vec3_reflect(Vec3 v, Vec3 n) {
    return vec3_sub(v, vec3_mul(n, 2.0f * vec3_dot(v, n)));
}

static inline Vec3 vec3_project(Vec3 a, Vec3 b) {
    float dot = vec3_dot(a, b);
    float len_sq = vec3_len_sq(b);
    return len_sq > LINALG_EPSILON ? vec3_mul(b, dot / len_sq) : vec3_zero();
}

static inline bool vec3_equal(Vec3 a, Vec3 b, float eps) {
    return fabsf(a.x - b.x) < eps && 
           fabsf(a.y - b.y) < eps && 
           fabsf(a.z - b.z) < eps;
}

// ============================================================================
// Vec4
// ============================================================================

typedef struct { float x, y, z, w; } Vec4;

static inline Vec4 vec4(float x, float y, float z, float w) {
    return (Vec4){x, y, z, w};
}

static inline Vec4 vec4_from_vec3(Vec3 v, float w) {
    return (Vec4){v.x, v.y, v.z, w};
}

static inline Vec3 vec4_to_vec3(Vec4 v) {
    return (Vec3){v.x, v.y, v.z};
}

static inline Vec4 vec4_add(Vec4 a, Vec4 b) {
    return (Vec4){a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w};
}

static inline Vec4 vec4_mul(Vec4 a, float s) {
    return (Vec4){a.x * s, a.y * s, a.z * s, a.w * s};
}

static inline float vec4_dot(Vec4 a, Vec4 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

// ============================================================================
// Mat4 (Column-major 4x4 matrix)
// ============================================================================

typedef struct {
    float m[16];  // Column-major: m[col*4 + row]
} Mat4;

static inline Mat4 mat4_identity(void) {
    return (Mat4){{
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    }};
}

static inline Mat4 mat4_mul(Mat4 a, Mat4 b) {
    Mat4 r;
    for (int col = 0; col < 4; col++) {
        for (int row = 0; row < 4; row++) {
            float sum = 0;
            for (int i = 0; i < 4; i++) {
                sum += a.m[i * 4 + row] * b.m[col * 4 + i];
            }
            r.m[col * 4 + row] = sum;
        }
    }
    return r;
}

static inline Vec4 mat4_mul_vec4(Mat4 m, Vec4 v) {
    return (Vec4){
        m.m[0] * v.x + m.m[4] * v.y + m.m[8]  * v.z + m.m[12] * v.w,
        m.m[1] * v.x + m.m[5] * v.y + m.m[9]  * v.z + m.m[13] * v.w,
        m.m[2] * v.x + m.m[6] * v.y + m.m[10] * v.z + m.m[14] * v.w,
        m.m[3] * v.x + m.m[7] * v.y + m.m[11] * v.z + m.m[15] * v.w
    };
}

static inline Vec3 mat4_mul_vec3(Mat4 m, Vec3 v) {
    Vec4 v4 = vec4_from_vec3(v, 1.0f);
    Vec4 r = mat4_mul_vec4(m, v4);
    return vec4_to_vec3(r);
}

static inline Mat4 mat4_translate(Vec3 v) {
    Mat4 m = mat4_identity();
    m.m[12] = v.x;
    m.m[13] = v.y;
    m.m[14] = v.z;
    return m;
}

static inline Mat4 mat4_scale(Vec3 v) {
    return (Mat4){{
        v.x, 0,   0,   0,
        0,   v.y, 0,   0,
        0,   0,   v.z, 0,
        0,   0,   0,   1
    }};
}

static inline Mat4 mat4_rotate_x(float angle) {
    float c = cosf(angle), s = sinf(angle);
    return (Mat4){{
        1, 0,  0, 0,
        0, c,  s, 0,
        0, -s, c, 0,
        0, 0,  0, 1
    }};
}

static inline Mat4 mat4_rotate_y(float angle) {
    float c = cosf(angle), s = sinf(angle);
    return (Mat4){{
        c, 0, -s, 0,
        0, 1, 0,  0,
        s, 0, c,  0,
        0, 0, 0,  1
    }};
}

static inline Mat4 mat4_rotate_z(float angle) {
    float c = cosf(angle), s = sinf(angle);
    return (Mat4){{
        c,  s, 0, 0,
        -s, c, 0, 0,
        0,  0, 1, 0,
        0,  0, 0, 1
    }};
}

static inline Mat4 mat4_perspective(float fov, float aspect, float near, float far) {
    float f = 1.0f / tanf(fov / 2.0f);
    float range_inv = 1.0f / (near - far);
    
    return (Mat4){{
        f / aspect, 0, 0, 0,
        0, f, 0, 0,
        0, 0, (near + far) * range_inv, -1,
        0, 0, 2 * near * far * range_inv, 0
    }};
}

static inline Mat4 mat4_look_at(Vec3 eye, Vec3 center, Vec3 up) {
    Vec3 f = vec3_normalize(vec3_sub(center, eye));
    Vec3 s = vec3_normalize(vec3_cross(f, up));
    Vec3 u = vec3_cross(s, f);
    
    Mat4 m = mat4_identity();
    m.m[0] = s.x;
    m.m[4] = s.y;
    m.m[8] = s.z;
    m.m[1] = u.x;
    m.m[5] = u.y;
    m.m[9] = u.z;
    m.m[2] = -f.x;
    m.m[6] = -f.y;
    m.m[10] = -f.z;
    m.m[12] = -vec3_dot(s, eye);
    m.m[13] = -vec3_dot(u, eye);
    m.m[14] = vec3_dot(f, eye);
    
    return m;
}

#endif // LINALG_H
