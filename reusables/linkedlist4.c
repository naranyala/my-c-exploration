typedef struct List {
  struct List *next;
  // your data here or use containers
} List;

#define LIST_FOREACH(head, iter)                                               \
  for (List *iter = (head); iter != NULL; iter = iter->next)

static void list_push(List **head, List *node) {
  node->next = *head;
  *head = node;
}
