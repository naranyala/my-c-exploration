typedef struct {
  char *start;
  char *end;
  char *next;
  char sep;
} StrSplit;

void split_init(StrSplit *it, char *s, char sep) {
  it->start = it->next = s;
  it->sep = sep;
  it->end = s + strlen(s);
}
int split_next(StrSplit *it, char **tok, size_t *len) {
  // returns 0 when done
}
