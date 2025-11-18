
#include <stdio.h>
#include <stdlib.h>
#define TABLE_SIZE 10

typedef struct HashNode {
    int key;
    int value;
    struct HashNode *next;
} HashNode;

typedef struct HashTable {
    HashNode *table[TABLE_SIZE];
} HashTable;

// Initialize hash table
void initializeHashTable(HashTable *ht) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        ht->table[i] = NULL;
    }
}

// Hash function
unsigned int hashFunction(int key) {
    return key % TABLE_SIZE;
}

// Insert a key-value pair
void insert(HashTable *ht, int key, int value) {
    unsigned int index = hashFunction(key);
    HashNode *newNode = (HashNode *)malloc(sizeof(HashNode));
    newNode->key = key;
    newNode->value = value;
    newNode->next = ht->table[index];
    ht->table[index] = newNode;
}

// Search for a key
int search(HashTable *ht, int key) {
    unsigned int index = hashFunction(key);
    HashNode *current = ht->table[index];
    while (current != NULL) {
        if (current->key == key) {
            return current->value;
        }
        current = current->next;
    }
    return -1; // Not found
}
