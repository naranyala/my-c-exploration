#include "scic.h"
#include <math.h>
#include <stdio.h>

double f(double x) { return x * x - 2; } /* x^2 - 2 = 0  =>  x=sqrt(2) */

int main(void) {
  /* 1. array math */
  sci_arr x = sci_range(0, M_PI, 11);
  sci_arr y = sci_zeros(x.n);
  sci_sin(&x, &y); /* y = sin(x)   */
  printf("∫₀^π sin(x) dx ≈ %.6f  (exact 2.0)\n", sci_trapz(&x, &y));

  /* 2. root finding */
  double root = sci_brent(f, 0, 2, 1e-10);
  printf("sqrt(2) via brent = %.10f\n", root);

  /* 3. 3×3 linear system */
  double A[9] = {2, 1, 1, 1, 3, 2, 1, 0, 0};
  double b[3] = {4, 5, 1}, sol[3];
  sci_solve3(A, b, sol);
  printf("solve 3×3: [%g %g %g]\n", sol[0], sol[1], sol[2]);

  /* 4. FFT */
  size_t N = 16;
  double complex z[16];
  for (size_t i = 0; i < N; ++i)
    z[i] = sin(2 * M_PI * i / N);
  sci_fft(N, z, -1); /* forward */
  printf("FFT bin 1 magnitude = %g\n", cabs(z[1]) / (N / 2));

  sci_free(&x);
  sci_free(&y);
  return 0;
}
