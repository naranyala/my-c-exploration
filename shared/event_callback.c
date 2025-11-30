#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef void (*EventHandler)(void *data);

typedef struct EventListener {
  EventHandler handler;
  void *user_data;
  struct EventListener *next;
} EventListener;

typedef struct {
  EventListener *listeners;
} EventSystem;

EventSystem *event_system_create() { return calloc(1, sizeof(EventSystem)); }

void event_system_subscribe(EventSystem *es, EventHandler handler,
                            void *user_data) {
  EventListener *listener = malloc(sizeof(EventListener));
  listener->handler = handler;
  listener->user_data = user_data;
  listener->next = es->listeners;
  es->listeners = listener;
}

void event_system_emit(EventSystem *es, void *event_data) {
  EventListener *current = es->listeners;
  while (current) {
    current->handler(current->user_data);
    current = current->next;
  }
}

void event_system_free(EventSystem *es) {
  EventListener *current = es->listeners;
  while (current) {
    EventListener *next = current->next;
    free(current);
    current = next;
  }
  free(es);
}

// Example handlers
void on_button_click(void *data) {
  printf("Button clicked! Data: %s\n", (char *)data);
}

void on_key_press(void *data) {
  printf("Key pressed! Code: %d\n", *(int *)data);
}

int main() {
  EventSystem *es = event_system_create();

  char *button_data = "Submit Button";
  int key_code = 13; // Enter key

  event_system_subscribe(es, on_button_click, button_data);
  event_system_subscribe(es, on_key_press, &key_code);

  event_system_emit(es, NULL); // Event data not used in handlers

  event_system_free(es);
  return 0;
}
