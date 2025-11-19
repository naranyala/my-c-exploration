#include "strbuilder.h"
#include <stdio.h>

int main() {
  strbuilder_t sb = strbuilder_new("Request: ");

  strbuilder_append(&sb, "GET ");
  strbuilder_appendf(&sb, "/user/%d", 42);
  strbuilder_append(&sb, "?name=");
  strbuilder_append(&sb, "Grok"); // URL encode if you want, but raw here

  strbuilder_appendf(&sb, " [time=%.3fms]", 12.345);

  printf("Final URL: %s\n",
         strbuilder_view(&sb)); // peek without taking ownership

  char *final = strbuilder_build(&sb); // takes ownership
  printf("Owned string: %s\n", final);

  free(final); // because we called strbuilder_build
  // sb.data is now garbage — don't use sb after build() unless you reset

  return 0;
}
