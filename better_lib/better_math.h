/*
 * better_math.h - Single-header math utilities
 * 
 * Usage: Just #include "better_math.h"
 * 
 * Define BETTER_MATH_NO_SHORT_NAMES before including to avoid
 * polluting namespace (use bm_clamp instead of clamp, etc.)
 */

#ifndef BETTER_MATH_H
#define BETTER_MATH_H

#include <math.h>
#include <float.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Portable M_PI - not guaranteed by C standard */
#ifndef BM_PI
    #ifdef M_PI
        #define BM_PI M_PI
    #else
        #define BM_PI 3.14159265358979323846
    #endif
#endif

#define BM_TAU      (2.0 * BM_PI)
#define BM_PI_2     (BM_PI / 2.0)
#define BM_DEG2RAD  (BM_PI / 180.0)
#define BM_RAD2DEG  (180.0 / BM_PI)

/* ── Core Functions ─────────────────────────────────────────────── */

/* Clamp value between min and max (handles NaN by returning min) */
static inline double bm_clamp(double x, double min, double max) {
    if (!(x >= min)) return min;  /* Catches NaN and x < min */
    if (x > max) return max;
    return x;
}

static inline float bm_clampf(float x, float min, float max) {
    if (!(x >= min)) return min;
    if (x > max) return max;
    return x;
}

static inline int bm_clampi(int x, int min, int max) {
    if (x < min) return min;
    if (x > max) return max;
    return x;
}

/* Linear interpolation: lerp(a, b, 0) = a, lerp(a, b, 1) = b */
static inline double bm_lerp(double a, double b, double t) {
    return a + (b - a) * t;
}

static inline float bm_lerpf(float a, float b, float t) {
    return a + (b - a) * t;
}

/* Clamped lerp (t is clamped to [0, 1]) */
static inline double bm_lerp_clamped(double a, double b, double t) {
    return bm_lerp(a, b, bm_clamp(t, 0.0, 1.0));
}

/* Inverse lerp: returns t such that lerp(a, b, t) = value */
static inline double bm_inv_lerp(double a, double b, double value) {
    if (fabs(b - a) < DBL_EPSILON) return 0.0;  /* Avoid division by zero */
    return (value - a) / (b - a);
}

/* Remap value from [in_min, in_max] to [out_min, out_max] */
static inline double bm_remap(double value, double in_min, double in_max,
                              double out_min, double out_max) {
    double t = bm_inv_lerp(in_min, in_max, value);
    return bm_lerp(out_min, out_max, t);
}

/* ── Angle Conversions ──────────────────────────────────────────── */

static inline double bm_deg2rad(double deg) { return deg * BM_DEG2RAD; }
static inline double bm_rad2deg(double rad) { return rad * BM_RAD2DEG; }
static inline float bm_deg2radf(float deg) { return deg * (float)BM_DEG2RAD; }
static inline float bm_rad2degf(float rad) { return rad * (float)BM_RAD2DEG; }

/* Normalize angle to [0, 360) degrees */
static inline double bm_normalize_deg(double deg) {
    deg = fmod(deg, 360.0);
    return deg < 0.0 ? deg + 360.0 : deg;
}

/* Normalize angle to [-PI, PI) radians */
static inline double bm_normalize_rad(double rad) {
    rad = fmod(rad + BM_PI, BM_TAU);
    return rad < 0.0 ? rad + BM_PI : rad - BM_PI;
}

/* ── Utility Functions ──────────────────────────────────────────── */

static inline double bm_min(double a, double b) { return a < b ? a : b; }
static inline double bm_max(double a, double b) { return a > b ? a : b; }
static inline int bm_mini(int a, int b) { return a < b ? a : b; }
static inline int bm_maxi(int a, int b) { return a > b ? a : b; }

static inline double bm_sign(double x) {
    return (x > 0.0) - (x < 0.0);
}

static inline int bm_signi(int x) {
    return (x > 0) - (x < 0);
}

/* Step function: returns 0 if x < edge, else 1 */
static inline double bm_step(double edge, double x) {
    return x < edge ? 0.0 : 1.0;
}

/* Smooth step (cubic Hermite interpolation) */
static inline double bm_smoothstep(double edge0, double edge1, double x) {
    double t = bm_clamp((x - edge0) / (edge1 - edge0), 0.0, 1.0);
    return t * t * (3.0 - 2.0 * t);
}

/* Approximate equality for floating point */
static inline int bm_approx_eq(double a, double b, double epsilon) {
    return fabs(a - b) <= epsilon;
}

static inline int bm_approx_eqf(float a, float b, float epsilon) {
    return fabsf(a - b) <= epsilon;
}

/* ── Short Names (optional) ─────────────────────────────────────── */

#ifndef BETTER_MATH_NO_SHORT_NAMES
    #define clamp       bm_clamp
    #define clampf      bm_clampf
    #define clampi      bm_clampi
    #define lerp        bm_lerp
    #define lerpf       bm_lerpf
    #define lerp_clamped bm_lerp_clamped
    #define inv_lerp    bm_inv_lerp
    #define remap       bm_remap
    #define deg2rad     bm_deg2rad
    #define rad2deg     bm_rad2deg
    #define deg2radf    bm_deg2radf
    #define rad2degf    bm_rad2degf
    #define normalize_deg bm_normalize_deg
    #define normalize_rad bm_normalize_rad
    #define sign        bm_sign
    #define signi       bm_signi
    #define step        bm_step
    #define smoothstep  bm_smoothstep
    #define approx_eq   bm_approx_eq
    #define approx_eqf  bm_approx_eqf
    /* Note: min/max not aliased - too likely to conflict */
#endif

#ifdef __cplusplus
}
#endif

#endif /* BETTER_MATH_H */
