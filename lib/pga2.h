
#ifndef PGA2_H
#define PGA2_H

#ifdef __cplusplus
extern "C" {
#endif

// =======================================================
// pga2.h -- Tiny Extended 2D Projective Geometric Algebra
// Single-header, no deps.
//
// Usage:
//   #include "pga2.h"
//   // in one .c file before including, add:
//   #define PGA2_IMPLEMENTATION
//   #include "pga2.h"
//
// This file provides:
//  - mv (multivector) type (8 doubles): [1, e0, e1, e2, e01, e02, e12, e012]
//  - geometric product (gp)
//  - reverse, dual, inner, outer (wedge), grade ops
//  - rotor-based 2D rotation (sandwiching)
//  - convenience: point<->coords, line from two points, intersection,
//  projection
//  - debug helpers: print, is_point, is_line
//
// Notes:
//  - The representation follows the previous simple layout:
//      pga_point(x,y): e12 = 1, e02 = x, e01 = y
//      pga_line(a,b,c): e1 = a, e2 = b, e0 = c   (ax + by + c = 0)
//  - Many GA-heavy ops are implemented simply and robustly for the
//    2D projective case used for geometry tasks.
//
// =======================================================

#include <math.h>
#include <stdbool.h>
#include <stdio.h>

// ---- Multivector type ----
typedef struct {
  double c[8]; // [1, e0, e1, e2, e01, e02, e12, e012]
} mv;

// index shorthand
enum { IDX_S = 0, IDX_E0, IDX_E1, IDX_E2, IDX_E01, IDX_E02, IDX_E12, IDX_E012 };

// ---------- Basic constructors ----------
static inline mv pga2_zero() {
  mv m = {{0, 0, 0, 0, 0, 0, 0, 0}};
  return m;
}

static inline mv pga2_scalar(double s) {
  mv m = pga2_zero();
  m.c[IDX_S] = s;
  return m;
}

// Point at (x, y) in homogeneous PGA2
static inline mv pga2_point(double x, double y) {
  mv p = pga2_zero();
  p.c[IDX_E12] = 1.0; // homogeneous weight
  p.c[IDX_E02] = x;
  p.c[IDX_E01] = y;
  return p;
}

// Line ax + by + c = 0  -> e1=a, e2=b, e0=c
static inline mv pga2_line(double a, double b, double c) {
  mv L = pga2_zero();
  L.c[IDX_E1] = a;
  L.c[IDX_E2] = b;
  L.c[IDX_E0] = c;
  return L;
}

// Direction / ideal vector
static inline mv pga2_direction(double dx, double dy) {
  mv d = pga2_zero();
  d.c[IDX_E1] = dx;
  d.c[IDX_E2] = dy;
  return d;
}

// scalar from multivector
static inline double pga2_scalar_of(mv m) { return m.c[IDX_S]; }

// ---------- Basic arithmetic ----------
static inline mv pga2_add(mv a, mv b) {
  mv r;
  for (int i = 0; i < 8; ++i)
    r.c[i] = a.c[i] + b.c[i];
  return r;
}
static inline mv pga2_sub(mv a, mv b) {
  mv r;
  for (int i = 0; i < 8; ++i)
    r.c[i] = a.c[i] - b.c[i];
  return r;
}
static inline mv pga2_scale(mv a, double s) {
  mv r;
  for (int i = 0; i < 8; ++i)
    r.c[i] = a.c[i] * s;
  return r;
}

// ---------- Geometric product (hard-coded) ----------
static inline mv pga2_gp(mv A, mv B) {
  mv r = pga2_zero();
  const double *x = A.c, *y = B.c;

  // scalar
  r.c[IDX_S] = x[0] * y[0] + x[1] * y[1] - x[2] * y[2] - x[3] * y[3] -
               x[4] * y[4] - x[5] * y[5] - x[6] * y[6] - x[7] * y[7];

  // e0
  r.c[IDX_E0] = x[0] * y[1] + x[1] * y[0] - x[2] * y[4] - x[3] * y[5] -
                x[4] * y[2] - x[5] * y[3] - x[7] * y[6] + x[6] * y[7];

  // e1
  r.c[IDX_E1] = x[0] * y[2] + x[2] * y[0] + x[1] * y[4] - x[3] * y[6] +
                x[4] * y[1] - x[6] * y[3] - x[7] * y[5] + x[5] * y[7];

  // e2
  r.c[IDX_E2] = x[0] * y[3] + x[3] * y[0] + x[1] * y[5] + x[2] * y[6] +
                x[5] * y[1] + x[6] * y[2] - x[7] * y[4] + x[4] * y[7];

  // e01
  r.c[IDX_E01] = x[0] * y[4] + x[4] * y[0] + x[1] * y[2] - x[2] * y[1] -
                 x[3] * y[7] - x[7] * y[3] + x[5] * y[6] + x[6] * y[5];

  // e02
  r.c[IDX_E02] = x[0] * y[5] + x[5] * y[0] + x[1] * y[3] - x[3] * y[1] -
                 x[2] * y[7] - x[7] * y[2] + x[4] * y[6] + x[6] * y[4];

  // e12
  r.c[IDX_E12] = x[0] * y[6] + x[6] * y[0] + x[2] * y[3] - x[3] * y[2] +
                 x[1] * y[7] + x[7] * y[1] - x[4] * y[5] - x[5] * y[4];

  // e012
  r.c[IDX_E012] = x[0] * y[7] + x[7] * y[0] + x[1] * y[6] + x[6] * y[1] +
                  x[2] * y[5] + x[5] * y[2] + x[3] * y[4] + x[4] * y[3];

  return r;
}

// ---------- Reverse (reversion) ----------
// Reverse multiplies grade-k by (-1)^{k*(k-1)/2}.
// For k=0 => +1, k=1 => +1, k=2 => -1, k=3 => -1
static inline mv pga2_reverse(mv a) {
  mv r = a;
  // negate bivector (e01,e02,e12) and trivector e012
  r.c[IDX_E01] = -r.c[IDX_E01];
  r.c[IDX_E02] = -r.c[IDX_E02];
  r.c[IDX_E12] = -r.c[IDX_E12];
  r.c[IDX_E012] = -r.c[IDX_E012];
  return r;
}

// ---------- Dual / undual ----------
// Dual by right-multiplying by pseudoscalar I = e012 (choice matches common PGA
// conv).
static inline mv pga2_pseudoscalar() {
  mv I = pga2_zero();
  I.c[IDX_E012] = 1.0;
  return I;
}
static inline mv pga2_dual(mv a) {
  // a * I
  return pga2_gp(a, pga2_pseudoscalar());
}
static inline mv pga2_undual(mv a) {
  // a * I (since I*I = 0-ish in degenerate metric, undual is same op up to sign
  // in PGA2)
  return pga2_gp(a, pga2_pseudoscalar());
}

// ---------- Inner & Outer (wedge) ----------
// For our small PGA2 and for convenience we implement:
//  outer(a,b)   := grade((a*b), grade(a)+grade(b))
//  inner(a,b)   := a · b = grade((a*b), |grade(a)-grade(b)|)
// To avoid a heavy generic grade system, provide simple helpers for common
// uses.
//
// For many point/line uses, direct gp gives desired meet/join behavior.
//
// We'll provide a simple generic grade-extraction utility below and then
// outer/inner that use gp + projection.

static inline mv pga2_grade_project(mv a, int grade) {
  mv r = pga2_zero();
  // grade mapping:
  // grade-0: IDX_S
  // grade-1: IDX_E0, IDX_E1, IDX_E2
  // grade-2: IDX_E01, IDX_E02, IDX_E12
  // grade-3: IDX_E012
  if (grade == 0)
    r.c[IDX_S] = a.c[IDX_S];
  else if (grade == 1) {
    r.c[IDX_E0] = a.c[IDX_E0];
    r.c[IDX_E1] = a.c[IDX_E1];
    r.c[IDX_E2] = a.c[IDX_E2];
  } else if (grade == 2) {
    r.c[IDX_E01] = a.c[IDX_E01];
    r.c[IDX_E02] = a.c[IDX_E02];
    r.c[IDX_E12] = a.c[IDX_E12];
  } else if (grade == 3)
    r.c[IDX_E012] = a.c[IDX_E012];
  return r;
}

static inline mv pga2_outer(mv a, mv b) {
  // outer = grade_{ga+gb}(a*b) where ga,gb approximated by extracting max grade
  // component We'll compute grades by seeing which grade has the largest
  // magnitude component.
  auto grade_of = (mv m)->int {
    double g0 = fabs(m.c[IDX_S]);
    double g1 = fabs(m.c[IDX_E0]) + fabs(m.c[IDX_E1]) + fabs(m.c[IDX_E2]);
    double g2 = fabs(m.c[IDX_E01]) + fabs(m.c[IDX_E02]) + fabs(m.c[IDX_E12]);
    double g3 = fabs(m.c[IDX_E012]);
    if (g3 >= g2 && g3 >= g1 && g3 >= g0)
      return 3;
    if (g2 >= g1 && g2 >= g0)
      return 2;
    if (g1 >= g0)
      return 1;
    return 0;
  };
  int ga = grade_of(a);
  int gb = grade_of(b);
  int g = ga + gb;
  mv prod = pga2_gp(a, b);
  if (g > 3)
    return pga2_zero();
  return pga2_grade_project(prod, g);
}

static inline mv pga2_inner(mv a, mv b) {
  // inner = grade_{|ga-gb|}(a*b)
  auto grade_of = (mv m)->int {
    double g0 = fabs(m.c[IDX_S]);
    double g1 = fabs(m.c[IDX_E0]) + fabs(m.c[IDX_E1]) + fabs(m.c[IDX_E2]);
    double g2 = fabs(m.c[IDX_E01]) + fabs(m.c[IDX_E02]) + fabs(m.c[IDX_E12]);
    double g3 = fabs(m.c[IDX_E012]);
    if (g3 >= g2 && g3 >= g1 && g3 >= g0)
      return 3;
    if (g2 >= g1 && g2 >= g0)
      return 2;
    if (g1 >= g0)
      return 1;
    return 0;
  };
  int ga = grade_of(a);
  int gb = grade_of(b);
  int g = (ga > gb) ? (ga - gb) : (gb - ga);
  mv prod = pga2_gp(a, b);
  if (g > 3)
    return pga2_zero();
  return pga2_grade_project(prod, g);
}

// ---------- Utilities: conversions to/from coordinates ----------

// extract (x,y) from point mv (works for finite point where e12!=0)
static inline bool pga2_point_to_xy(mv P, double *out_x, double *out_y) {
  double w = P.c[IDX_E12];
  if (w == 0.0)
    return false; // ideal point or invalid
  *out_x = P.c[IDX_E02] / w;
  *out_y = P.c[IDX_E01] / w;
  return true;
}

// extract (a,b,c) from line mv where line = a e1 + b e2 + c e0
static inline void pga2_line_to_abc(mv L, double *out_a, double *out_b,
                                    double *out_c) {
  *out_a = L.c[IDX_E1];
  *out_b = L.c[IDX_E2];
  *out_c = L.c[IDX_E0];
}

// create line from two finite points using determinant method
static inline mv pga2_line_from_points(mv A, mv B) {
  double ax, ay, bx, by;
  bool okA = pga2_point_to_xy(A, &ax, &ay);
  bool okB = pga2_point_to_xy(B, &bx, &by);
  if (!okA || !okB) {
    // fallback: use gp which for homogeneous points gives their join
    return pga2_gp(A, B);
  }
  // line coefficients for line through (ax,ay) and (bx,by):
  // a = ay - by; b = bx - ax; c = ax*by - bx*ay
  double a = ay - by;
  double b = bx - ax;
  double c = ax * by - bx * ay;
  return pga2_line(a, b, c);
}

// intersection of two lines (returns point in homogeneous form)
// If parallel, returned point will be ideal (e12==0)
static inline mv pga2_intersect_lines(mv L1, mv L2) {
  // Solve:
  // a1 x + b1 y + c1 = 0
  // a2 x + b2 y + c2 = 0
  double a1 = L1.c[IDX_E1], b1 = L1.c[IDX_E2], c1 = L1.c[IDX_E0];
  double a2 = L2.c[IDX_E1], b2 = L2.c[IDX_E2], c2 = L2.c[IDX_E0];
  double det = a1 * b2 - a2 * b1;
  if (fabs(det) < 1e-15) {
    // parallel: return ideal point (direction)
    // direction = (b1, -a1) (perp to normal) or use cross product of lines in
    // homogeneous coordinates: Using gp (line1 ^ line2) produces a point
    // (possibly ideal)
    return pga2_gp(L1, L2);
  } else {
    double x = (b1 * c2 - b2 * c1) / det;
    double y = (a2 * c1 - a1 * c2) / det;
    return pga2_point(x, y);
  }
}

// distance from point to line (absolute signed distance)
static inline double pga2_distance_point_line(mv P, mv L) {
  double x, y;
  if (!pga2_point_to_xy(P, &x, &y)) {
    // ideal point distance undefined; return nan
    return NAN;
  }
  double a = L.c[IDX_E1], b = L.c[IDX_E2], c = L.c[IDX_E0];
  double denom = sqrt(a * a + b * b);
  if (denom == 0.0)
    return NAN;
  return fabs(a * x + b * y + c) / denom;
}

// projection of point P onto line L -> returns a new point
static inline mv pga2_project_point_on_line(mv P, mv L) {
  double x, y;
  if (!pga2_point_to_xy(P, &x, &y)) {
    // ideal point project -> return original
    return P;
  }
  double a = L.c[IDX_E1], b = L.c[IDX_E2], c = L.c[IDX_E0];
  double denom = a * a + b * b;
  if (denom == 0.0)
    return P;
  double t = (a * x + b * y + c) / denom;
  double xr = x - a * t;
  double yr = y - b * t;
  return pga2_point(xr, yr);
}

// project point P onto nearest point on segment AB (convenience)
static inline mv pga2_project_point_on_segment(mv P, mv A, mv B) {
  // project to infinite line then clamp to segment endpoints
  mv L = pga2_line_from_points(A, B);
  mv proj = pga2_project_point_on_line(P, L);
  double px, py, ax, ay, bx, by, prx, pry;
  pga2_point_to_xy(P, &px, &py);
  pga2_point_to_xy(A, &ax, &ay);
  pga2_point_to_xy(B, &bx, &by);
  pga2_point_to_xy(proj, &prx, &pry);
  // barycentric test: check projection lies between A and B
  double dotABAB = (bx - ax) * (bx - ax) + (by - ay) * (by - ay);
  if (dotABAB == 0.0)
    return A;
  double t = ((prx - ax) * (bx - ax) + (pry - ay) * (by - ay)) / dotABAB;
  if (t < 0.0)
    return A;
  if (t > 1.0)
    return B;
  return proj;
}

// translate a point by (dx,dy)
static inline mv pga2_translate_point(mv P, double dx, double dy) {
  // For our point representation, adding to e02,e01 works (since P: e12=1,
  // e02=x, e01=y)
  mv r = P;
  r.c[IDX_E02] += dx * r.c[IDX_E12];
  r.c[IDX_E01] += dy * r.c[IDX_E12];
  return r;
}

// rotate around origin by angle radians using rotor R = cos(a/2) + sin(a/2)*e12
// apply: P' = R * P * ~R
static inline mv pga2_make_rotor(double angle) {
  double c = cos(angle * 0.5);
  double s = sin(angle * 0.5);
  mv R = pga2_zero();
  R.c[IDX_S] = c;
  R.c[IDX_E12] = s;
  return R;
}
static inline mv pga2_rotate_point_about_origin(mv P, double angle) {
  mv R = pga2_make_rotor(angle);
  mv Rt = pga2_reverse(R);
  mv t = pga2_gp(pga2_gp(R, P), Rt);
  return t;
}

// rotate point around arbitrary center C: translate to origin, rotate,
// translate back
static inline mv pga2_rotate_point_around(mv P, mv C, double angle) {
  double cx, cy;
  if (!pga2_point_to_xy(C, &cx, &cy)) {
    // if C not finite, fallback to origin rotation
    return pga2_rotate_point_about_origin(P, angle);
  }
  mv tmp = pga2_translate_point(P, -cx, -cy);
  tmp = pga2_rotate_point_about_origin(tmp, angle);
  tmp = pga2_translate_point(tmp, cx, cy);
  return tmp;
}

// ---------- Debug / Safety helpers ----------
static inline void pga2_print(mv a) {
  printf("mv { s=%g, e0=%g, e1=%g, e2=%g, e01=%g, e02=%g, e12=%g, e012=%g }\n",
         a.c[IDX_S], a.c[IDX_E0], a.c[IDX_E1], a.c[IDX_E2], a.c[IDX_E01],
         a.c[IDX_E02], a.c[IDX_E12], a.c[IDX_E012]);
}

static inline bool pga2_is_point(mv a) {
  // basic heuristic: e12 != 0 indicates finite point (weight)
  return fabs(a.c[IDX_E12]) > 0.0;
}
static inline bool pga2_is_line(mv a) {
  // heuristic: has e1 or e2 or e0 set
  return (fabs(a.c[IDX_E1]) > 0.0) || (fabs(a.c[IDX_E2]) > 0.0) ||
         (fabs(a.c[IDX_E0]) > 0.0);
}

// ---------- Numeric helpers ----------
static inline double pga2_length_direction(mv d) {
  // direction stored as e1,e2
  return sqrt(d.c[IDX_E1] * d.c[IDX_E1] + d.c[IDX_E2] * d.c[IDX_E2]);
}

// ---------- Convenience aliases (non-prefixed names for compatibility)
// ----------
#define mv                                                                     \
  pga2_mv_alias_placeholder /* keep mv type name as-is (type name already mv)  \
                             */

// original-style function names mapped to pga2_ prefixed ones for compatibility
#define gp(a, b) pga2_gp(a, b)
#define join(a, b) pga2_gp(a, b)
#define meet(a, b) pga2_gp(a, b)
#define point(x, y) pga2_point(x, y)
#define line(a, b, c) pga2_line(a, b, c)
#define translate_point(p, dx, dy) pga2_translate_point(p, dx, dy)
#define rotate_point_about_origin(p, ang) pga2_rotate_point_about_origin(p, ang)

// ---------- End of header public API ----------

#ifdef __cplusplus
}
#endif

#endif // PGA2_H

// =======================================================
// Implementation section (only compiled when PGA2_IMPLEMENTATION defined)
// Put "#define PGA2_IMPLEMENTATION" in ONE C file before including this header
// =======================================================
#ifdef PGA2_IMPLEMENTATION

// Nothing additional required — all functions inlined above.
// But we keep this section to allow future non-inlined code and to avoid
// multiple-definition issues if you later move non-inline implementations here.

#endif // PGA2_IMPLEMENTATION
