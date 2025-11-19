
#include <stdio.h>
#include <stdlib.h>

int **createMatrix(int rows, int cols) {
  int **mat = (int **)malloc(rows * sizeof(int *));
  for (int i = 0; i < rows; i++)
    mat[i] = (int *)malloc(cols * sizeof(int));
  return mat;
}

void freeMatrix(int **mat, int rows) {
  for (int i = 0; i < rows; i++)
    free(mat[i]);
  free(mat);
}

int main() {
  int rows = 3, cols = 3;
  int **matrix = createMatrix(rows, cols);

  // Fill matrix
  for (int i = 0; i < rows; i++)
    for (int j = 0; j < cols; j++)
      matrix[i][j] = i * cols + j;

  // Print matrix
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++)
      printf("%d ", matrix[i][j]);
    printf("\n");
  }

  freeMatrix(matrix, rows);
  return 0;
}
