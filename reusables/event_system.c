/**
 * Event-driven system with callbacks
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_EVENTS 10
#define MAX_SUBSCRIBERS_PER_EVENT 5

typedef enum {
  EVENT_BUTTON_PRESS,
  EVENT_TIMER_EXPIRED,
  EVENT_DATA_RECEIVED,
  EVENT_SYSTEM_SHUTDOWN,
  EVENT_TYPE_COUNT
} EventType;

typedef struct {
  EventType type;
  void *data;
  size_t data_size;
} Event;

typedef void (*EventHandler)(const Event *);

typedef struct {
  EventHandler handlers[MAX_SUBSCRIBERS_PER_EVENT];
  int handler_count;
} EventSubscribers;

typedef struct {
  EventSubscribers subscribers[EVENT_TYPE_COUNT];
} EventSystem;

EventSystem *event_system_create(void) {
  EventSystem *es = malloc(sizeof(EventSystem));
  if (!es)
    return NULL;

  for (int i = 0; i < EVENT_TYPE_COUNT; i++) {
    es->subscribers[i].handler_count = 0;
  }

  return es;
}

int event_system_subscribe(EventSystem *es, EventType type,
                           EventHandler handler) {
  if (type >= EVENT_TYPE_COUNT)
    return 0;

  EventSubscribers *subs = &es->subscribers[type];
  if (subs->handler_count >= MAX_SUBSCRIBERS_PER_EVENT)
    return 0;

  subs->handlers[subs->handler_count++] = handler;
  return 1;
}

void event_system_publish(EventSystem *es, const Event *event) {
  if (event->type >= EVENT_TYPE_COUNT)
    return;

  EventSubscribers *subs = &es->subscribers[event->type];
  for (int i = 0; i < subs->handler_count; i++) {
    subs->handlers[i](event);
  }
}

void event_system_destroy(EventSystem *es) { free(es); }

// Example event handlers
void button_press_handler(const Event *event) {
  printf("Button pressed! Data: %s\n", (char *)event->data);
}

void timer_handler(const Event *event) {
  int *counter = (int *)event->data;
  printf("Timer expired! Counter: %d\n", *counter);
}

void data_received_handler(const Event *event) {
  char *data = (char *)event->data;
  printf("Data received: %s\n", data);
}

void shutdown_handler(const Event *event) {
  printf("System shutdown requested\n");
}

// Example usage
int main(void) {
  EventSystem *es = event_system_create();

  // Subscribe to events
  event_system_subscribe(es, EVENT_BUTTON_PRESS, button_press_handler);
  event_system_subscribe(es, EVENT_TIMER_EXPIRED, timer_handler);
  event_system_subscribe(es, EVENT_DATA_RECEIVED, data_received_handler);
  event_system_subscribe(es, EVENT_SYSTEM_SHUTDOWN, shutdown_handler);

  // Publish events
  Event event;
  char button_data[] = "OK Button";
  int timer_counter = 42;
  char received_data[] = "Hello, Event System!";

  event.type = EVENT_BUTTON_PRESS;
  event.data = button_data;
  event.data_size = sizeof(button_data);
  event_system_publish(es, &event);

  event.type = EVENT_TIMER_EXPIRED;
  event.data = &timer_counter;
  event.data_size = sizeof(timer_counter);
  event_system_publish(es, &event);

  event.type = EVENT_DATA_RECEIVED;
  event.data = received_data;
  event.data_size = sizeof(received_data);
  event_system_publish(es, &event);

  event.type = EVENT_SYSTEM_SHUTDOWN;
  event.data = NULL;
  event.data_size = 0;
  event_system_publish(es, &event);

  event_system_destroy(es);
  return 0;
}
