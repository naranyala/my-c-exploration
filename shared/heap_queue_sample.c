
#include <stdio.h>
#include <stdlib.h>
#define MAX_HEAP_SIZE 100

typedef struct MinHeap {
    int *array;
    int size;
    int capacity;
} MinHeap;

// Initialize heap
void initializeHeap(MinHeap *heap, int capacity) {
    heap->array = (int *)malloc(capacity * sizeof(int));
    heap->size = 0;
    heap->capacity = capacity;
}

// Insert a value into the heap
void insertHeap(MinHeap *heap, int value) {
    if (heap->size == heap->capacity) {
        printf("Heap is full!\n");
        return;
    }
    heap->array[heap->size++] = value;
    int i = heap->size - 1;
    while (i != 0 && heap->array[(i - 1) / 2] > heap->array[i]) {
        int temp = heap->array[i];
        heap->array[i] = heap->array[(i - 1) / 2];
        heap->array[(i - 1) / 2] = temp;
        i = (i - 1) / 2;
    }
}

// Extract the minimum value
int extractMin(MinHeap *heap) {
    if (heap->size == 0) {
        printf("Heap is empty!\n");
        return -1;
    }
    int min = heap->array[0];
    heap->array[0] = heap->array[--heap->size];
    int i = 0;
    while (1) {
        int left = 2 * i + 1;
        int right = 2 * i + 2;
        int smallest = i;
        if (left < heap->size && heap->array[left] < heap->array[smallest]) {
            smallest = left;
        }
        if (right < heap->size && heap->array[right] < heap->array[smallest]) {
            smallest = right;
        }
        if (smallest == i) break;
        int temp = heap->array[i];
        heap->array[i] = heap->array[smallest];
        heap->array[smallest] = temp;
        i = smallest;
    }
    return min;
}
