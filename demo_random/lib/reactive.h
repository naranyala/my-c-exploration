#ifndef REACTIVE_H
#define REACTIVE_H

#include <stdio.h>
#include <stdlib.h>

// --- Reactive Infrastructure ---

// Observer callback now receives the Subject's data AND the Observer's own
// context
typedef void (*ObserverCallback)(void *subjectData, void *observerContext);

typedef struct {
  ObserverCallback callback;
  void *context;
} ObserverEntry;

typedef struct {
  void *data;
  ObserverEntry *observers;
  int observerCount;
  int observerCapacity;
} ReactiveState;

static inline void InitReactiveState(ReactiveState *state, void *data) {
  state->data = data;
  state->observers = NULL;
  state->observerCount = 0;
  state->observerCapacity = 0;
}

static inline void Subscribe(ReactiveState *state, ObserverCallback callback,
                             void *context) {
  if (state->observerCount >= state->observerCapacity) {
    int newCapacity =
        (state->observerCapacity == 0) ? 4 : state->observerCapacity * 2;
    ObserverEntry *newObservers = (ObserverEntry *)realloc(
        state->observers, newCapacity * sizeof(ObserverEntry));
    if (!newObservers) {
      fprintf(stderr, "Error: Failed to allocate memory for observers\n");
      return;
    }
    state->observers = newObservers;
    state->observerCapacity = newCapacity;
  }

  state->observers[state->observerCount].callback = callback;
  state->observers[state->observerCount].context = context;
  state->observerCount++;
}

static inline void Notify(ReactiveState *state) {
  for (int i = 0; i < state->observerCount; i++) {
    if (state->observers[i].callback) {
      state->observers[i].callback(state->data, state->observers[i].context);
    }
  }
}

static inline void SetState(ReactiveState *state) { Notify(state); }

static inline void CleanupReactiveState(ReactiveState *state) {
  if (state->observers) {
    free(state->observers);
    state->observers = NULL;
  }
  state->observerCount = 0;
  state->observerCapacity = 0;
}

#endif // REACTIVE_H
