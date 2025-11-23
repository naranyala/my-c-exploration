
#include "better_malloc2.h"

int main() {
    int *arr = malloc_or_die(10 * sizeof(int));
    arr[0] = 42;
    free(arr);
}
