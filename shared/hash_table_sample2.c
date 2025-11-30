/**
 * Simple hash table implementation
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TABLE_SIZE 100

typedef struct HashNode {
  char *key;
  int value;
  struct HashNode *next;
} HashNode;

typedef struct {
  HashNode **buckets;
  size_t size;
} HashTable;

unsigned int hash(const char *key) {
  unsigned int hash = 0;
  while (*key) {
    hash = (hash << 5) + *key++;
  }
  return hash % TABLE_SIZE;
}

HashTable *hash_table_create(void) {
  HashTable *table = malloc(sizeof(HashTable));
  if (!table)
    return NULL;

  table->buckets = calloc(TABLE_SIZE, sizeof(HashNode *));
  if (!table->buckets) {
    free(table);
    return NULL;
  }

  table->size = 0;
  return table;
}

void hash_table_put(HashTable *table, const char *key, int value) {
  unsigned int index = hash(key);

  HashNode *new_node = malloc(sizeof(HashNode));
  new_node->key = strdup(key);
  new_node->value = value;
  new_node->next = table->buckets[index];

  table->buckets[index] = new_node;
  table->size++;
}

int hash_table_get(HashTable *table, const char *key, int *value) {
  unsigned int index = hash(key);
  HashNode *current = table->buckets[index];

  while (current) {
    if (strcmp(current->key, key) == 0) {
      *value = current->value;
      return 1;
    }
    current = current->next;
  }
  return 0;
}

void hash_table_destroy(HashTable *table) {
  for (int i = 0; i < TABLE_SIZE; i++) {
    HashNode *current = table->buckets[i];
    while (current) {
      HashNode *temp = current;
      current = current->next;
      free(temp->key);
      free(temp);
    }
  }
  free(table->buckets);
  free(table);
}

// Example usage
int main(void) {
  HashTable *table = hash_table_create();

  hash_table_put(table, "apple", 10);
  hash_table_put(table, "banana", 20);
  hash_table_put(table, "cherry", 30);

  int value;
  if (hash_table_get(table, "banana", &value)) {
    printf("banana: %d\n", value);
  }

  hash_table_destroy(table);
  return 0;
}
