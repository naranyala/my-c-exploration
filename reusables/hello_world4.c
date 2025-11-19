/**
 * File: program_template.c
 * Description: Basic C program structure template
 * Author: Your Name
 * Date: 2024
 */

#include <stdio.h>
#include <stdlib.h>

// Function prototypes
void function1(void);
int function2(int param);

int main(void) {
  printf("Program started\n");

  function1();
  int result = function2(10);
  printf("Result: %d\n", result);

  printf("Program finished\n");
  return 0;
}

// Function definitions
void function1(void) { printf("Inside function1\n"); }

int function2(int param) { return param * 2; }
