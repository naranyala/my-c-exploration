/*
 * dynarray_demo.c — demo using updated dynarray.h
 *
 * Compile:
 *   gcc -std=c11 dynarray_demo.c -O2 -o dynarray_demo
 *
 * The updated da_foreach_ptr declares the pointer variable internally,
 * so you do NOT need to declare `pptr` before using the macro.
 */

#include "dynarray.h"
#include <stdio.h>

typedef struct {
  int x, y;
} point;

int main(void) {
  da_t(int) nums = NULL;
  da_push(nums, 1);
  da_push(nums, 2);
  da_push(nums, 3);

  da_t(point) pts = NULL;
  da_push(pts, (point){.x = 10, .y = 20});
  da_push(pts, (point){.x = 30, .y = 40});

  /* pointer iteration — `pptr` is declared by the macro */
  da_foreach_ptr(point, pptr, pts) {
    pptr->x += 1;
    pptr->y += 1;
  }

  for (size_t i = 0; i < da_count(pts); ++i) {
    printf("pt[%zu] = (%d,%d)\n", i, pts[i].x, pts[i].y);
  }

  da_free(nums);
  da_free(pts);
  return 0;
}
