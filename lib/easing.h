// easing.h - Collection of easing functions for animations
// Usage:
//   float t = 0.5f; // 0 to 1
//   float eased = ease_out_bounce(t);

#ifndef EASING_H
#define EASING_H

#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Linear
static inline float ease_linear(float t) {
    return t;
}

// Quadratic
static inline float ease_in_quad(float t) {
    return t * t;
}

static inline float ease_out_quad(float t) {
    return 1 - (1 - t) * (1 - t);
}

static inline float ease_in_out_quad(float t) {
    return t < 0.5f ? 2 * t * t : 1 - powf(-2 * t + 2, 2) / 2;
}

// Cubic
static inline float ease_in_cubic(float t) {
    return t * t * t;
}

static inline float ease_out_cubic(float t) {
    return 1 - powf(1 - t, 3);
}

static inline float ease_in_out_cubic(float t) {
    return t < 0.5f ? 4 * t * t * t : 1 - powf(-2 * t + 2, 3) / 2;
}

// Quartic
static inline float ease_in_quart(float t) {
    return t * t * t * t;
}

static inline float ease_out_quart(float t) {
    return 1 - powf(1 - t, 4);
}

static inline float ease_in_out_quart(float t) {
    return t < 0.5f ? 8 * t * t * t * t : 1 - powf(-2 * t + 2, 4) / 2;
}

// Sine
static inline float ease_in_sine(float t) {
    return 1 - cosf((t * M_PI) / 2);
}

static inline float ease_out_sine(float t) {
    return sinf((t * M_PI) / 2);
}

static inline float ease_in_out_sine(float t) {
    return -(cosf(M_PI * t) - 1) / 2;
}

// Exponential
static inline float ease_in_expo(float t) {
    return t == 0 ? 0 : powf(2, 10 * t - 10);
}

static inline float ease_out_expo(float t) {
    return t == 1 ? 1 : 1 - powf(2, -10 * t);
}

static inline float ease_in_out_expo(float t) {
    return t == 0 ? 0 : t == 1 ? 1 : t < 0.5f ? 
           powf(2, 20 * t - 10) / 2 : (2 - powf(2, -20 * t + 10)) / 2;
}

// Elastic
static inline float ease_in_elastic(float t) {
    float c4 = (2 * M_PI) / 3;
    return t == 0 ? 0 : t == 1 ? 1 : 
           -powf(2, 10 * t - 10) * sinf((t * 10 - 10.75f) * c4);
}

static inline float ease_out_elastic(float t) {
    float c4 = (2 * M_PI) / 3;
    return t == 0 ? 0 : t == 1 ? 1 : 
           powf(2, -10 * t) * sinf((t * 10 - 0.75f) * c4) + 1;
}

static inline float ease_in_out_elastic(float t) {
    float c5 = (2 * M_PI) / 4.5f;
    return t == 0 ? 0 : t == 1 ? 1 : t < 0.5f ?
           -(powf(2, 20 * t - 10) * sinf((20 * t - 11.125f) * c5)) / 2 :
           (powf(2, -20 * t + 10) * sinf((20 * t - 11.125f) * c5)) / 2 + 1;
}

// Bounce - define ease_out_bounce first since others depend on it
static inline float ease_out_bounce(float t) {
    float n1 = 7.5625f;
    float d1 = 2.75f;
    
    if (t < 1 / d1) {
        return n1 * t * t;
    } else if (t < 2 / d1) {
        t -= 1.5f / d1;
        return n1 * t * t + 0.75f;
    } else if (t < 2.5f / d1) {
        t -= 2.25f / d1;
        return n1 * t * t + 0.9375f;
    } else {
        t -= 2.625f / d1;
        return n1 * t * t + 0.984375f;
    }
}

static inline float ease_in_bounce(float t) {
    return 1 - ease_out_bounce(1 - t);
}

static inline float ease_in_out_bounce(float t) {
    return t < 0.5f ? (1 - ease_out_bounce(1 - 2 * t)) / 2 :
           (1 + ease_out_bounce(2 * t - 1)) / 2;
}

// Back
static inline float ease_in_back(float t) {
    float c1 = 1.70158f;
    float c3 = c1 + 1;
    return c3 * t * t * t - c1 * t * t;
}

static inline float ease_out_back(float t) {
    float c1 = 1.70158f;
    float c3 = c1 + 1;
    return 1 + c3 * powf(t - 1, 3) + c1 * powf(t - 1, 2);
}

static inline float ease_in_out_back(float t) {
    float c1 = 1.70158f;
    float c2 = c1 * 1.525f;
    return t < 0.5f ?
           (powf(2 * t, 2) * ((c2 + 1) * 2 * t - c2)) / 2 :
           (powf(2 * t - 2, 2) * ((c2 + 1) * (t * 2 - 2) + c2) + 2) / 2;
}

#endif // EASING_H
