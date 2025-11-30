#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_CAPACITY 16
#define LOAD_FACTOR_THRESHOLD 0.75

typedef struct HashNode {
  char *key;
  void *value;
  struct HashNode *next;
} HashNode;

typedef struct {
  HashNode **buckets;
  size_t size;
  size_t capacity;
} HashTable;

unsigned int hash(const char *key, size_t capacity) {
  unsigned int hash_value = 0;
  while (*key) {
    hash_value = hash_value * 31 + *key;
    key++;
  }
  return hash_value % capacity;
}

HashTable *ht_create() {
  HashTable *ht = malloc(sizeof(HashTable));
  ht->capacity = INITIAL_CAPACITY;
  ht->size = 0;
  ht->buckets = calloc(ht->capacity, sizeof(HashNode *));
  return ht;
}

void ht_put(HashTable *ht, const char *key, void *value) {
  unsigned int index = hash(key, ht->capacity);

  HashNode *node = ht->buckets[index];
  while (node) {
    if (strcmp(node->key, key) == 0) {
      node->value = value; // Update existing
      return;
    }
    node = node->next;
  }

  // Add new node
  HashNode *new_node = malloc(sizeof(HashNode));
  new_node->key = strdup(key);
  new_node->value = value;
  new_node->next = ht->buckets[index];
  ht->buckets[index] = new_node;
  ht->size++;
}

void *ht_get(HashTable *ht, const char *key) {
  unsigned int index = hash(key, ht->capacity);

  HashNode *node = ht->buckets[index];
  while (node) {
    if (strcmp(node->key, key) == 0) {
      return node->value;
    }
    node = node->next;
  }
  return NULL;
}

void ht_free(HashTable *ht) {
  for (size_t i = 0; i < ht->capacity; i++) {
    HashNode *node = ht->buckets[i];
    while (node) {
      HashNode *temp = node;
      node = node->next;
      free(temp->key);
      free(temp);
    }
  }
  free(ht->buckets);
  free(ht);
}

int main() {
  HashTable *ht = ht_create();

  char *value1 = "Hello";
  char *value2 = "World";

  ht_put(ht, "greeting", value1);
  ht_put(ht, "subject", value2);

  printf("greeting: %s\n", (char *)ht_get(ht, "greeting"));
  printf("subject: %s\n", (char *)ht_get(ht, "subject"));

  ht_free(ht);
  return 0;
}
