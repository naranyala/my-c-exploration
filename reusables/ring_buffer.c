typedef struct {
  void *data;
  size_t head, tail, size, capacity;
  size_t elem_size;
} RingBuffer;

#define rb_full(rb) (((rb)->tail + 1) % (rb)->capacity == (rb)->head)
#define rb_empty(rb) ((rb)->head == (rb)->tail)
