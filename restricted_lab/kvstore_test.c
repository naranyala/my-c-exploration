#include "lib/kvstore.h"
#include "raylib.h"
#include <assert.h>
#include <stdio.h>

void print_entry(const char *key, KVValue val, void *ctx) {
  int *count = (int *)ctx;
  (*count)++;
  printf("Key: %s, Type: %d\n", key, val.type);
}

int main() {
  printf("Running KVStore Tests...\n");

  KVStore *store = KV_Create();
  assert(store != NULL);

  // 1. Basic Types
  printf("Test 1: Basic Types... ");
  KV_SetInt(store, "score", 100);
  KV_SetFloat(store, "health", 95.5f);
  KV_SetString(store, "name", "Player1");
  Vector2 pos = {10.0f, 20.0f};
  KV_SetVec2(store, "position", pos);

  assert(KV_GetInt(store, "score", 0) == 100);
  assert(KV_GetFloat(store, "health", 0.0f) == 95.5f);
  assert(strcmp(KV_GetString(store, "name", ""), "Player1") == 0);
  Vector2 p = KV_GetVec2(store, "position", (Vector2){0, 0});
  assert(p.x == 10.0f && p.y == 20.0f);
  printf("PASSED\n");

  // 2. Overwrite
  printf("Test 2: Overwrite... ");
  KV_SetInt(store, "score", 200);
  assert(KV_GetInt(store, "score", 0) == 200);

  KV_SetString(store, "name", "Player2"); // Should free "Player1"
  assert(strcmp(KV_GetString(store, "name", ""), "Player2") == 0);
  printf("PASSED\n");

  // 3. Deletion
  printf("Test 3: Deletion... ");
  assert(KV_Delete(store, "score") == true);
  assert(KV_Has(store, "score") == false);
  assert(KV_GetInt(store, "score", -1) == -1);
  printf("PASSED\n");

  // 4. Resizing (Force many inserts)
  printf("Test 4: Resizing... ");
  char keyBuf[32];
  for (int i = 0; i < 100; i++) {
    snprintf(keyBuf, 32, "key_%d", i);
    KV_SetInt(store, keyBuf, i);
  }

  // Verify all exist
  for (int i = 0; i < 100; i++) {
    snprintf(keyBuf, 32, "key_%d", i);
    assert(KV_GetInt(store, keyBuf, -1) == i);
  }
  printf("PASSED (Count: %d, Capacity: %d)\n", store->count, store->capacity);

  // 5. Iteration
  printf("Test 5: Iteration... ");
  int count = 0;
  KV_ForEach(store, print_entry, &count);
  // We deleted "score", added 3 others (health, name, position), then added 100
  // keys. Total should be 103.
  assert(count == 103);
  printf("PASSED\n");

  KV_Destroy(store);
  printf("All Tests PASSED!\n");
  return 0;
}
