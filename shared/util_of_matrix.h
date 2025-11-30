
// matrix.h
#ifndef MATRIX_H
#define MATRIX_H

#include <stdlib.h>

typedef struct {
  size_t rows, cols;
  double *data;
} Matrix;

static inline Matrix *matrix_new(size_t r, size_t c) {
  Matrix *m = malloc(sizeof(Matrix));
  m->rows = r;
  m->cols = c;
  m->data = calloc(r * c, sizeof(double));
  return m;
}

static inline double matrix_get(Matrix *m, size_t r, size_t c) {
  return m->data[r * m->cols + c];
}

static inline void matrix_set(Matrix *m, size_t r, size_t c, double val) {
  m->data[r * m->cols + c] = val;
}

static inline void matrix_free(Matrix *m) {
  free(m->data);
  free(m);
}

#endif
