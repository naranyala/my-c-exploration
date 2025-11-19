typedef struct {
  size_t count;
  int values[]; // flexible array member
} IntArray;

IntArray *intarray_new(size_t n) {
  IntArray *a = malloc(sizeof(IntArray) + n * sizeof(int));
  a->count = n;
  return a;
}
