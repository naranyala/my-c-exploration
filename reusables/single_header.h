
// > utils.h
#ifndef UTILS_H
#define UTILS_H

void greet(const char *name);

#endif

#include <stdio.h>

void greet(const char *name) { printf("Hello, %s!\n", name); }

// > main.c
// #include "utils.h"
//
// int main() {
//     greet("Alice");
//     return 0;
// }
//
