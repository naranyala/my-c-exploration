#include <stdio.h>
#define DHASH_IMPL
#include "dhash.h"

int main(void) {
  dhash_set("name", (void *)"Alice");
  dhash_set("age", (void *)25); // cast int to void* (only safe for small ints)

  printf("Name: %s\n", (char *)dhash_get("name"));
  printf("Age: %d\n", (int)dhash_get("age"));

  dhash_del("name");
  if (!dhash_get("name")) {
    puts("Name removed!");
  }

  return 0;
}
