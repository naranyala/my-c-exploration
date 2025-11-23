/* Usage */
#include "ergo_hashmap.h"
#include <string.h>

int main() {
    hmap_t map = hmap_new();
    hmap_put(&map, "name", "Alice");
    hmap_put(&map, "age",  (void*)30);
    printf("Name: %s, Age: %ld\n", (char*)hmap_get(&map, "name"), (uintptr_t)hmap_get(&map, "age"));
    hmap_free(map);
}

