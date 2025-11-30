#include <execinfo.h>
void print_stack(void) {
  void *array[50];
  int size = backtrace(array, 50);
  char **strings = backtrace_symbols(array, size);
  for (int i = 0; i < size; i++)
    fprintf(stderr, "%s\n", strings[i]);
  free(strings);
}
