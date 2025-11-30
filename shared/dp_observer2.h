
// observer.h
#ifndef OBSERVER_H
#define OBSERVER_H

#include <stdlib.h>

typedef void (*Observer)(void *);

typedef struct {
  Observer *subs;
  size_t count;
} Subject;

static inline void subject_init(Subject *s) {
  s->subs = NULL;
  s->count = 0;
}

static inline void subject_attach(Subject *s, Observer o) {
  s->subs = realloc(s->subs, sizeof(Observer) * (s->count + 1));
  s->subs[s->count++] = o;
}

static inline void subject_notify(Subject *s, void *msg) {
  for (size_t i = 0; i < s->count; i++)
    s->subs[i](msg);
}

#endif
