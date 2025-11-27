#ifndef SIMPLE_EVENT_H
#define SIMPLE_EVENT_H

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*event_callback_t)(void* userdata, void* event_data);

typedef struct event_listener_s {
    event_callback_t callback;
    void* userdata;
    struct event_listener_s* next;
} event_listener_t;

typedef struct {
    event_listener_t* listeners;
} event_t;

void event_init(event_t* event);
void event_add_listener(event_t* event, event_callback_t cb, void* userdata);
void event_remove_listener(event_t* event, event_callback_t cb, void* userdata);
void event_fire(event_t* event, void* event_data);

#ifdef __cplusplus
}
#endif

#endif // SIMPLE_EVENT_H

#ifdef SIMPLE_EVENT_IMPLEMENTATION

void event_init(event_t* event) {
    event->listeners = NULL;
}

void event_add_listener(event_t* event, event_callback_t cb, void* userdata) {
    if (!event || !cb) return;
    event_listener_t* listener = (event_listener_t*)malloc(sizeof(event_listener_t));
    listener->callback = cb;
    listener->userdata = userdata;
    listener->next = event->listeners;
    event->listeners = listener;
}

void event_remove_listener(event_t* event, event_callback_t cb, void* userdata) {
    if (!event || !cb) return;
    event_listener_t** cur = &event->listeners;
    while (*cur) {
        if ((*cur)->callback == cb && (*cur)->userdata == userdata) {
            event_listener_t* to_free = *cur;
            *cur = (*cur)->next;
            free(to_free);
            return;
        }
        cur = &(*cur)->next;
    }
}

void event_fire(event_t* event, void* event_data) {
    if (!event) return;
    for (event_listener_t* cur = event->listeners; cur; cur = cur->next) {
        cur->callback(cur->userdata, event_data);
    }
}

#endif

