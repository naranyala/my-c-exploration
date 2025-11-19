#define ONCE_INIT {.mutex = PTHREAD_MUTEX_INITIALIZER}

typedef struct {
  pthread_mutex_t mutex;
  int inited;
  SomeType value;
} Once;

void init_once(Once *o) {
  pthread_mutex_lock(&o->mutex);
  if (!o->inited) {
    o->value = really_expensive_init();
    o->inited = 1;
  }
  pthread_mutex_unlock(&o->mutex);
}
