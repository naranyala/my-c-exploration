/* gp3d_math.h - General Purpose 3D Math Utilities
 *
 * Features:
 * - Vector operations (float3)
 * - Matrix operations (4x4)
 * - Quaternion support
 * - Common 3D math utilities
 * - SIMD optimization (optional)
 * - C99 compliant
 */

#ifndef GP3D_MATH_H
#define GP3D_MATH_H

#include <math.h>
#include <string.h>
#include <float.h>

/* Configuration */
#ifndef GP3D_PI
    #define GP3D_PI 3.14159265358979323846f
#endif

#ifndef GP3D_EPSILON
    #define GP3D_EPSILON 1e-6f
#endif

/* Vector3 Structure */
typedef struct {
    float x, y, z;
} gp3d_vec3;

/* Matrix4 Structure (Column-major) */
typedef struct {
    float m[16]; // Column-major: m[column*4 + row]
} gp3d_mat4;

/* Quaternion Structure (x, y, z, w) */
typedef struct {
    float x, y, z, w;
} gp3d_quat;

/* ========== Vector3 Functions ========== */

/* Create vector from components */
static gp3d_vec3 gp3d_vec3_make(float x, float y, float z) {
    gp3d_vec3 v = {x, y, z};
    return v;
}

/* Vector operations */
static gp3d_vec3 gp3d_vec3_add(gp3d_vec3 a, gp3d_vec3 b) {
    return gp3d_vec3_make(a.x + b.x, a.y + b.y, a.z + b.z);
}

static gp3d_vec3 gp3d_vec3_sub(gp3d_vec3 a, gp3d_vec3 b) {
    return gp3d_vec3_make(a.x - b.x, a.y - b.y, a.z - b.z);
}

static gp3d_vec3 gp3d_vec3_scale(gp3d_vec3 v, float s) {
    return gp3d_vec3_make(v.x * s, v.y * s, v.z * s);
}

static float gp3d_vec3_dot(gp3d_vec3 a, gp3d_vec3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

static gp3d_vec3 gp3d_vec3_cross(gp3d_vec3 a, gp3d_vec3 b) {
    return gp3d_vec3_make(
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    );
}

static float gp3d_vec3_length(gp3d_vec3 v) {
    return sqrtf(gp3d_vec3_dot(v, v));
}

static gp3d_vec3 gp3d_vec3_normalize(gp3d_vec3 v) {
    float len = gp3d_vec3_length(v);
    if (len > GP3D_EPSILON) {
        return gp3d_vec3_scale(v, 1.0f / len);
    }
    return gp3d_vec3_make(0, 0, 0);
}

static float gp3d_vec3_distance(gp3d_vec3 a, gp3d_vec3 b) {
    gp3d_vec3 diff = gp3d_vec3_sub(a, b);
    return gp3d_vec3_length(diff);
}

/* ========== Matrix4 Functions ========== */

/* Create identity matrix */
static gp3d_mat4 gp3d_mat4_identity(void) {
    gp3d_mat4 m = {{
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    }};
    return m;
}

/* Create translation matrix */
static gp3d_mat4 gp3d_mat4_translate(float x, float y, float z) {
    gp3d_mat4 m = gp3d_mat4_identity();
    m.m[12] = x;
    m.m[13] = y;
    m.m[14] = z;
    return m;
}

/* Create rotation matrix (angle in radians) */
static gp3d_mat4 gp3d_mat4_rotate(float angle, float x, float y, float z) {
    gp3d_mat4 m = gp3d_mat4_identity();
    float len = sqrtf(x*x + y*y + z*z);
    if (len < GP3D_EPSILON) return m;
    
    float s = sinf(angle);
    float c = cosf(angle);
    float one_minus_c = 1.0f - c;
    
    // Normalize axis
    x /= len; y /= len; z /= len;
    
    m.m[0] = x*x*one_minus_c + c;
    m.m[1] = y*x*one_minus_c + z*s;
    m.m[2] = z*x*one_minus_c - y*s;
    
    m.m[4] = x*y*one_minus_c - z*s;
    m.m[5] = y*y*one_minus_c + c;
    m.m[6] = z*y*one_minus_c + x*s;
    
    m.m[8] = x*z*one_minus_c + y*s;
    m.m[9] = y*z*one_minus_c - x*s;
    m.m[10] = z*z*one_minus_c + c;
    
    return m;
}

/* Create scaling matrix */
static gp3d_mat4 gp3d_mat4_scale(float x, float y, float z) {
    gp3d_mat4 m = gp3d_mat4_identity();
    m.m[0] = x;
    m.m[5] = y;
    m.m[10] = z;
    return m;
}

/* Matrix multiplication (a * b) */
static gp3d_mat4 gp3d_mat4_mul(gp3d_mat4 a, gp3d_mat4 b) {
    gp3d_mat4 result;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            float sum = 0.0f;
            for (int k = 0; k < 4; k++) {
                sum += a.m[i*4 + k] * b.m[k*4 + j];
            }
            result.m[i*4 + j] = sum;
        }
    }
    return result;
}

/* Transform vector by matrix (for points: w=1, vectors: w=0) */
static gp3d_vec3 gp3d_mat4_transform_vec3(gp3d_mat4 m, gp3d_vec3 v, float w) {
    gp3d_vec3 result;
    result.x = m.m[0]*v.x + m.m[4]*v.y + m.m[8]*v.z + m.m[12]*w;
    result.y = m.m[1]*v.x + m.m[5]*v.y + m.m[9]*v.z + m.m[13]*w;
    result.z = m.m[2]*v.x + m.m[6]*v.y + m.m[10]*v.z + m.m[14]*w;
    return result;
}

/* Perspective projection matrix */
static gp3d_mat4 gp3d_mat4_perspective(float fov, float aspect, float near, float far) {
    gp3d_mat4 m = {{0}};
    float tan_half_fov = tanf(fov * 0.5f);
    
    m.m[0] = 1.0f / (aspect * tan_half_fov);
    m.m[5] = 1.0f / tan_half_fov;
    m.m[10] = -(far + near) / (far - near);
    m.m[11] = -1.0f;
    m.m[14] = -(2.0f * far * near) / (far - near);
    
    return m;
}

/* Look-at matrix (camera transformation) */
static gp3d_mat4 gp3d_mat4_look_at(
    gp3d_vec3 eye, gp3d_vec3 center, gp3d_vec3 up
) {
    gp3d_vec3 f = gp3d_vec3_normalize(gp3d_vec3_sub(center, eye));
    gp3d_vec3 s = gp3d_vec3_normalize(gp3d_vec3_cross(f, up));
    gp3d_vec3 u = gp3d_vec3_cross(s, f);
    
    gp3d_mat4 m = {{0}};
    m.m[0] = s.x;  m.m[4] = s.y;  m.m[8] = s.z;
    m.m[1] = u.x;  m.m[5] = u.y;  m.m[9] = u.z;
    m.m[2] = -f.x; m.m[6] = -f.y; m.m[10] = -f.z;
    m.m[12] = -gp3d_vec3_dot(s, eye);
    m.m[13] = -gp3d_vec3_dot(u, eye);
    m.m[14] = gp3d_vec3_dot(f, eye);
    m.m[15] = 1.0f;
    
    return m;
}

/* ========== Quaternion Functions ========== */

/* Create quaternion from axis-angle */
static gp3d_quat gp3d_quat_from_axis_angle(float angle, float x, float y, float z) {
    float len = sqrtf(x*x + y*y + z*z);
    if (len < GP3D_EPSILON) {
        return (gp3d_quat){0, 0, 0, 1};
    }
    
    float half_angle = angle * 0.5f;
    float s = sinf(half_angle) / len;
    
    return (gp3d_quat){
        x * s,
        y * s,
        z * s,
        cosf(half_angle)
    };
}

/* Quaternion multiplication */
static gp3d_quat gp3d_quat_mul(gp3d_quat q1, gp3d_quat q2) {
    return (gp3d_quat){
        q1.w*q2.x + q1.x*q2.w + q1.y*q2.z - q1.z*q2.y,
        q1.w*q2.y - q1.x*q2.z + q1.y*q2.w + q1.z*q2.x,
        q1.w*q2.z + q1.x*q2.y - q1.y*q2.x + q1.z*q2.w,
        q1.w*q2.w - q1.x*q2.x - q1.y*q2.y - q1.z*q2.z
    };
}

/* Convert quaternion to matrix */
static gp3d_mat4 gp3d_quat_to_mat4(gp3d_quat q) {
    float xx = q.x * q.x;
    float yy = q.y * q.y;
    float zz = q.z * q.z;
    float xy = q.x * q.y;
    float xz = q.x * q.z;
    float yz = q.y * q.z;
    float xw = q.x * q.w;
    float yw = q.y * q.w;
    float zw = q.z * q.w;
    
    return (gp3d_mat4){{
        1 - 2*(yy + zz), 2*(xy - zw),     2*(xz + yw),     0,
        2*(xy + zw),     1 - 2*(xx + zz), 2*(yz - xw),     0,
        2*(xz - yw),     2*(yz + xw),     1 - 2*(xx + yy), 0,
        0,               0,               0,               1
    }};
}

/* Normalize quaternion */
static gp3d_quat gp3d_quat_normalize(gp3d_quat q) {
    float len = sqrtf(q.x*q.x + q.y*q.y + q.z*q.z + q.w*q.w);
    if (len > GP3D_EPSILON) {
        float inv_len = 1.0f / len;
        return (gp3d_quat){q.x*inv_len, q.y*inv_len, q.z*inv_len, q.w*inv_len};
    }
    return (gp3d_quat){0, 0, 0, 1};
}

/* Spherical linear interpolation */
static gp3d_quat gp3d_quat_slerp(gp3d_quat q1, gp3d_quat q2, float t) {
    // Normalize inputs
    q1 = gp3d_quat_normalize(q1);
    q2 = gp3d_quat_normalize(q2);
    
    float dot = q1.x*q2.x + q1.y*q2.y + q1.z*q2.z + q1.w*q2.w;
    
    // If dot product is negative, negate one quaternion
    gp3d_quat end = q2;
    if (dot < 0.0f) {
        end = (gp3d_quat){-q2.x, -q2.y, -q2.z, -q2.w};
        dot = -dot;
    }
    
    // If quaternions are very close, use linear interpolation
    if (dot > 0.9995f) {
        return gp3d_quat_normalize((gp3d_quat){
            q1.x + t*(end.x - q1.x),
            q1.y + t*(end.y - q1.y),
            q1.z + t*(end.z - q1.z),
            q1.w + t*(end.w - q1.w)
        });
    }
    
    float theta = acosf(dot) * t;
    gp3d_quat q3 = gp3d_quat_normalize((gp3d_quat){
        end.x - q1.x*dot,
        end.y - q1.y*dot,
        end.z - q1.z*dot,
        end.w - q1.w*dot
    });
    
    return (gp3d_quat){
        q1.x*cosf(theta) + q3.x*sinf(theta),
        q1.y*cosf(theta) + q3.y*sinf(theta),
        q1.z*cosf(theta) + q3.z*sinf(theta),
        q1.w*cosf(theta) + q3.w*sinf(theta)
    };
}

#endif /* GP3D_MATH_H */
