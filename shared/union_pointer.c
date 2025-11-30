typedef union {
  int *i;
  float *f;
  void *v;
} any_ptr __attribute__((transparent_union));

void inc(any_ptr p) { (*p.i)++; } // works with int*, float*, etc.
