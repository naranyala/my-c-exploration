typedef struct {
  void (*func)(void *);
  void *arg;
} Task;

typedef struct {
  pthread_mutex_t lock;
  pthread_cond_t notify;
  pthread_t *threads;
  Task *tasks;
  int task_head, task_tail, task_count, task_cap;
  int shutdown;
} ThreadPool;

void tp_init(ThreadPool *tp, int nthreads);
void tp_submit(ThreadPool *tp, void (*func)(void *), void *arg);
void tp_destroy(ThreadPool *tp);
