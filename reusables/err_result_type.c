#include <stdio.h>
#include <stdlib.h>

typedef enum { RESULT_OK, RESULT_ERROR } ResultStatus;

typedef struct {
  ResultStatus status;
  int value;
  char *error_message;
} Result;

Result safe_divide(int a, int b) {
  Result result;
  if (b == 0) {
    result.status = RESULT_ERROR;
    result.error_message = "Division by zero";
    result.value = 0;
  } else {
    result.status = RESULT_OK;
    result.value = a / b;
    result.error_message = NULL;
  }
  return result;
}

int main() {
  Result r1 = safe_divide(10, 2);
  if (r1.status == RESULT_OK) {
    printf("10 / 2 = %d\n", r1.value);
  } else {
    printf("Error: %s\n", r1.error_message);
  }

  Result r2 = safe_divide(10, 0);
  if (r2.status == RESULT_OK) {
    printf("Result: %d\n", r2.value);
  } else {
    printf("Error: %s\n", r2.error_message);
  }

  return 0;
}
