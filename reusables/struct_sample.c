
#include <stdio.h>
#include <string.h>

typedef struct {
  char name[50];
  int age;
} Person;

int main() {
  Person p1;
  strcpy(p1.name, "Alice");
  p1.age = 25;

  printf("Name: %s, Age: %d\n", p1.name, p1.age);
  return 0;
}
