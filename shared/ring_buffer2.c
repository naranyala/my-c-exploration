typedef struct {
  uint8_t buffer[1 << 16];
  uint32_t head __attribute__((aligned(64)));
  uint32_t tail __attribute__((aligned(64)));
} SpscRing;

static inline int spsc_enqueue(SpscRing *r, const void *data, size_t len) {
  uint32_t head = __atomic_load_n(&r->head, __ATOMIC_RELAXED);
  uint32_t tail = __atomic_load_n(&r->tail, __ATOMIC_ACQUIRE);
  if (((head - tail) & 0xFFFF) > (0xFFFF - len))
    return -1; // full
  memcpy(r->buffer + (head & 0xFFFF), data, len);
  __atomic_store_n(&r->head, head + len, __ATOMIC_RELEASE);
  return 0;
}
