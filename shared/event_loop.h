
// eventloop.h
#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <unistd.h>

typedef void (*EventHandler)(void *);

typedef struct {
  int fd;
  EventHandler handler;
  void *arg;
} Event;

static inline void run_loop(Event *events, size_t count) {
  while (1) {
    fd_set set;
    FD_ZERO(&set);
    int maxfd = 0;
    for (size_t i = 0; i < count; i++) {
      FD_SET(events[i].fd, &set);
      if (events[i].fd > maxfd)
        maxfd = events[i].fd;
    }
    if (select(maxfd + 1, &set, NULL, NULL, NULL) > 0) {
      for (size_t i = 0; i < count; i++) {
        if (FD_ISSET(events[i].fd, &set)) {
          events[i].handler(events[i].arg);
        }
      }
    }
  }
}

#endif
