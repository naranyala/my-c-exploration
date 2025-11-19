typedef struct ListHead {
  struct ListHead *next, *prev;
} ListHead;

#define LIST_HEAD_INIT(name) {&(name), &(name)}

static inline void list_add(ListHead *new, ListHead *prev, ListHead *next) {
  next->prev = new;
  new->next = next;
  new->prev = prev;
  prev->next = new;
}

#define list_add_tail(new, head) list_add(new, (head)->prev, head)
#define list_add_head(new, head) list_add(new, head, (head)->next)

#define list_for_each_safe(pos, n, head)                                       \
  for (pos = (head)->next, n = pos->next; pos != (head); pos = n, n = pos->next)
