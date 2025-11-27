#ifndef C_MATH_H
#define C_MATH_H

#include <math.h> // For sinf, cosf, tanf, sqrtf

// --- Type Definitions ---

typedef struct { float x, y, z; } vec3;
typedef struct { float x, y, z, w; } vec4;
typedef struct { float m[16]; } mat4; // Column-major order (standard for OpenGL/DirectX)

// --- Function Declarations ---

// Vector Operations
vec3 vec3_add(vec3 a, vec3 b);
float vec3_dot(vec3 a, vec3 b);
float vec3_length(vec3 v);
vec3 vec3_normalize(vec3 v);

// Matrix Operations
mat4 mat4_identity(void);
mat4 mat4_multiply(mat4 a, mat4 b);
mat4 mat4_translate(mat4 m, vec3 v);
mat4 mat4_perspective(float fovy_rad, float aspect, float nearZ, float farZ);

// --- Implementation Section ---
#ifdef C_MATH_IMPLEMENTATION

#define C_MATH_PI 3.14159265359f

// --- Vector Implementation ---

vec3 vec3_add(vec3 a, vec3 b) {
    return (vec3){a.x + b.x, a.y + b.y, a.z + b.z};
}

float vec3_dot(vec3 a, vec3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

float vec3_length(vec3 v) {
    return sqrtf(vec3_dot(v, v));
}

vec3 vec3_normalize(vec3 v) {
    float len = vec3_length(v);
    if (len == 0.0f) return v;
    return (vec3){v.x / len, v.y / len, v.z / len};
}

// --- Matrix Implementation ---

mat4 mat4_identity(void) {
    mat4 m = {0};
    m.m[0] = m.m[5] = m.m[10] = m.m[15] = 1.0f;
    return m;
}

mat4 mat4_multiply(mat4 a, mat4 b) {
    mat4 result = {0};
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            for (int k = 0; k < 4; k++) {
                // result[i][j] = sum(a[i][k] * b[k][j])
                // Using 1D array access: m[c * 4 + r]
                result.m[j * 4 + i] += a.m[k * 4 + i] * b.m[j * 4 + k];
            }
        }
    }
    return result;
}

mat4 mat4_translate(mat4 m, vec3 v) {
    mat4 t = mat4_identity();
    t.m[12] = v.x; // Translation components in the 4th column (indices 12, 13, 14)
    t.m[13] = v.y;
    t.m[14] = v.z;
    return mat4_multiply(m, t);
}

mat4 mat4_perspective(float fovy_rad, float aspect, float nearZ, float farZ) {
    mat4 m = {0};
    float tan_half_fovy = tanf(fovy_rad / 2.0f);

    m.m[0] = 1.0f / (aspect * tan_half_fovy);
    m.m[5] = 1.0f / tan_half_fovy;
    m.m[10] = -(farZ + nearZ) / (farZ - nearZ); // Z-scaling
    m.m[11] = -1.0f;                             // Used for perspective divide
    m.m[14] = -(2.0f * farZ * nearZ) / (farZ - nearZ); // Z-translation (W component)
    
    return m;
}

#endif // C_MATH_IMPLEMENTATION

#endif // C_MATH_H
