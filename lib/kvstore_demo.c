#include "kvstore.h"
#include <stdio.h>

int main(void) {
  kvstore_t *kv = kvstore_create();

  kvstore_set(kv, "language", "C");
  kvstore_set(kv, "year", "2025");
  kvstore_set(kv, "author", "Grok");

  printf("language = %s\n", kvstore_get(kv, "language"));

  const char *k, *v;
  kvstore_iter_t it = kvstore_iter(kv);
  while (kvstore_iter_next(&it, &k, &v))
    printf("%s → %s\n", k, v);

  kvstore_destroy(kv);
  return 0;
}
