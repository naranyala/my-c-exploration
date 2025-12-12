#ifndef SCIC_H
#define SCIC_H
#include <complex.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

/* ---------- tiny vectorised array object ---------- */
typedef struct {
  size_t n;     /* number of elements */
  double *data; /* contiguous block    */
} sci_arr;

static inline sci_arr sci_zeros(size_t n) {
  sci_arr a = {n, (double *)calloc(n, sizeof(double))};
  return a;
}
static inline sci_arr sci_range(double start, double stop, size_t num) {
  sci_arr a = sci_zeros(num);
  double step = (stop - start) / (num - 1);
  for (size_t i = 0; i < num; ++i)
    a.data[i] = start + i * step;
  return a;
}
static inline void sci_free(sci_arr *a) { free(a->data); }

/* ---------- basic ufuncs ---------- */
static inline void sci_sin(const sci_arr *x, sci_arr *out) {
  for (size_t i = 0; i < x->n; ++i)
    out->data[i] = sin(x->data[i]);
}
static inline void sci_add(const sci_arr *a, const sci_arr *b, sci_arr *out) {
  for (size_t i = 0; i < a->n; ++i)
    out->data[i] = a->data[i] + b->data[i];
}

/* ---------- numerical integration (trapezoidal) ---------- */
static double sci_trapz(const sci_arr *x, const sci_arr *y) {
  double s = 0.0;
  for (size_t i = 1; i < x->n; ++i)
    s += (x->data[i] - x->data[i - 1]) * (y->data[i] + y->data[i - 1]);
  return s * 0.5;
}

/* ---------- root finding (Brent–Dekker) ---------- */
static double sci_brent(double (*f)(double), double a, double b, double tol) {
  double fa = f(a), fb = f(b), fc = fa;
  double c = a, d = b - a, e = d;
  if (fa * fb > 0)
    return NAN; /* no bracket */
  for (int iter = 0; iter < 100; ++iter) {
    if (fabs(fc) < fabs(fb)) {
      a = b;
      b = c;
      c = a;
      fa = fb;
      fb = fc;
      fc = fa;
    }
    const double m = 0.5 * (c - b);
    if (fabs(m) <= tol || fb == 0.0)
      return b;
    if (fabs(e) < tol || fabs(fa) <= fabs(fb)) {
      d = e = m;
    } /* bisect */
    else {
      double s = fb / fa;
      double p, q;
      if (a == c) {
        p = 2 * m * s;
        q = 1 - s;
      } /* secant */
      else {
        q = fa / fc;
        double r = fb / fc;
        p = s * (2 * m * q * (q - r) - (b - a) * (r - 1));
        q = (q - 1) * (r - 1) * (s - 1);
      }
      if (p > 0)
        q = -q;
      p = fabs(p);
      if (2 * p < fmin(3 * m * q - fabs(tol * q), fabs(e * q))) {
        e = d;
        d = p / q;
      } else {
        d = e = m;
      }
    }
    a = b;
    fa = fb;
    if (fabs(d) > tol)
      b += d;
    else
      b += (m > 0 ? +tol : -tol);
    fb = f(b);
    if ((fb > 0 && fc > 0) || (fb < 0 && fc < 0)) {
      c = a;
      fc = fa;
    }
  }
  return b;
}

/* ---------- 3×3 linear solve (CRAMER) ---------- */
static int sci_solve3(const double A[9], const double b[3], double x[3]) {
  double det = A[0] * (A[4] * A[8] - A[7] * A[5]) -
               A[1] * (A[3] * A[8] - A[5] * A[6]) +
               A[2] * (A[3] * A[7] - A[4] * A[6]);
  if (fabs(det) < 1e-14)
    return -1;
  x[0] =
      (b[0] * (A[4] * A[8] - A[7] * A[5]) - A[1] * (b[1] * A[8] - A[5] * b[2]) +
       A[2] * (b[1] * A[7] - A[4] * b[2])) /
      det;
  x[1] =
      (A[0] * (b[1] * A[8] - A[5] * b[2]) - b[0] * (A[3] * A[8] - A[5] * A[6]) +
       A[2] * (A[3] * b[2] - b[1] * A[6])) /
      det;
  x[2] =
      (A[0] * (A[4] * b[2] - A[7] * b[1]) - A[1] * (A[3] * b[2] - A[5] * b[0]) +
       b[0] * (A[3] * A[7] - A[4] * A[6])) /
      det;
  return 0;
}

/* ---------- radix-2 FFT (Cooley–Tukey, power-of-2 sizes) ---------- */
static void sci_fft(size_t n, double complex *inout, int sign) {
  if (n <= 1)
    return;
  double complex *tmp = (double complex *)malloc(n * sizeof(double complex));
  for (size_t i = 0; i < n; ++i)
    tmp[i] = inout[i];
  for (size_t i = 0; i < n; ++i) {
    size_t j = 0, k = i, bits = n >> 1;
    while (bits) {
      j = (j << 1) | (k & 1);
      k >>= 1;
      bits >>= 1;
    }
    inout[i] = tmp[j];
  }
  free(tmp);
  for (size_t len = 2; len <= n; len <<= 1) {
    double complex wlen = cexp(sign * 2 * M_PI * I / len);
    for (size_t i = 0; i < n; i += len) {
      double complex w = 1;
      for (size_t j = 0; j < len / 2; ++j) {
        double complex u = inout[i + j];
        double complex v = inout[i + j + len / 2] * w;
        inout[i + j] = u + v;
        inout[i + j + len / 2] = u - v;
        w *= wlen;
      }
    }
  }
}
#endif /* SCIC_H */
