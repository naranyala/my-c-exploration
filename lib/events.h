/*
 * events.h - A simple event system (pub/sub) for C
 * 
 * Usage:
 *   #define EVENTS_IMPLEMENTATION
 *   #include "events.h"
 * 
 * Features:
 *   - Publish-subscribe pattern
 *   - Type-safe event handlers
 *   - Multiple subscribers per event
 *   - Event data with type information
 *   - Thread-safe operations (with optional locking)
 * 
 * License: MIT
 */

#ifndef EVENTS_H
#define EVENTS_H

#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#ifdef EVENTS_THREAD_SAFE
    #ifdef _WIN32
        #include <windows.h>
    #else
        #include <pthread.h>
    #endif
#endif

typedef struct event_bus_t event_bus_t;
typedef struct event_t event_t;
typedef void (*event_handler_t)(const event_t* event);

#ifdef __cplusplus
extern "C" {
#endif

/* Event type definitions */
typedef enum {
    EVENT_TYPE_CUSTOM = 0,
    EVENT_TYPE_USER = 1000,  /* Start of user-defined types */
} event_type_t;

/* Event structure */
struct event_t {
    event_type_t type;
    const char* name;
    void* data;
    size_t data_size;
    void* user_context;
};

/* Event bus management */
event_bus_t* event_bus_create(void);
void event_bus_destroy(event_bus_t* bus);

/* Subscription management */
typedef size_t event_subscription_t;
event_subscription_t event_bus_subscribe(event_bus_t* bus, const char* event_name, 
                                       event_handler_t handler, void* user_context);
event_subscription_t event_bus_subscribe_type(event_bus_t* bus, event_type_t event_type,
                                            event_handler_t handler, void* user_context);
int event_bus_unsubscribe(event_bus_t* bus, event_subscription_t subscription);
void event_bus_unsubscribe_all(event_bus_t* bus, const char* event_name);
void event_bus_unsubscribe_all_type(event_bus_t* bus, event_type_t event_type);

/* Event publishing */
int event_bus_publish(event_bus_t* bus, const char* event_name, void* data, 
                     size_t data_size, void* user_context);
int event_bus_publish_type(event_bus_t* bus, event_type_t event_type, void* data,
                          size_t data_size, void* user_context);

/* Event creation (for advanced use) */
event_t* event_create(event_type_t type, const char* name, void* data, 
                     size_t data_size, void* user_context);
void event_destroy(event_t* event);

/* Utility functions */
size_t event_bus_subscriber_count(const event_bus_t* bus, const char* event_name);
size_t event_bus_subscriber_count_type(const event_bus_t* bus, event_type_t event_type);
void event_bus_set_global_context(event_bus_t* bus, void* global_context);
void* event_bus_get_global_context(const event_bus_t* bus);

/* Memory management for event data */
void* event_data_alloc(size_t size);
void event_data_free(void* data);

#ifdef __cplusplus
}
#endif

#endif /* EVENTS_H */

#ifdef EVENTS_IMPLEMENTATION

#include <assert.h>

typedef struct event_subscriber_t {
    event_subscription_t id;
    char* event_name;
    event_type_t event_type;
    event_handler_t handler;
    void* user_context;
    struct event_subscriber_t* next;
} event_subscriber_t;

struct event_bus_t {
    event_subscriber_t* subscribers;
    event_subscription_t next_subscription_id;
    void* global_context;
    
#ifdef EVENTS_THREAD_SAFE
    #ifdef _WIN32
        CRITICAL_SECTION lock;
    #else
        pthread_mutex_t lock;
    #endif
#endif
};

/* Internal utility functions */
static char* event_strdup(const char* str) {
    if (!str) return NULL;
    size_t len = strlen(str);
    char* copy = (char*)malloc(len + 1);
    if (copy) {
        memcpy(copy, str, len);
        copy[len] = '\0';
    }
    return copy;
}

#ifdef EVENTS_THREAD_SAFE
static void event_bus_lock(event_bus_t* bus) {
    #ifdef _WIN32
        EnterCriticalSection(&bus->lock);
    #else
        pthread_mutex_lock(&bus->lock);
    #endif
}

static void event_bus_unlock(event_bus_t* bus) {
    #ifdef _WIN32
        LeaveCriticalSection(&bus->lock);
    #else
        pthread_mutex_unlock(&bus->lock);
    #endif
}
#else
#define event_bus_lock(bus) ((void)0)
#define event_bus_unlock(bus) ((void)0)
#endif

event_bus_t* event_bus_create(void) {
    event_bus_t* bus = (event_bus_t*)calloc(1, sizeof(event_bus_t));
    if (!bus) return NULL;
    
    bus->next_subscription_id = 1;
    
#ifdef EVENTS_THREAD_SAFE
    #ifdef _WIN32
        InitializeCriticalSection(&bus->lock);
    #else
        pthread_mutex_init(&bus->lock, NULL);
    #endif
#endif
    
    return bus;
}

void event_bus_destroy(event_bus_t* bus) {
    if (!bus) return;
    
    event_bus_unsubscribe_all(bus, NULL);
    
#ifdef EVENTS_THREAD_SAFE
    #ifdef _WIN32
        DeleteCriticalSection(&bus->lock);
    #else
        pthread_mutex_destroy(&bus->lock);
    #endif
#endif
    
    free(bus);
}

event_subscription_t event_bus_subscribe(event_bus_t* bus, const char* event_name, 
                                       event_handler_t handler, void* user_context) {
    if (!bus || !event_name || !handler) return 0;
    
    event_subscriber_t* subscriber = (event_subscriber_t*)calloc(1, sizeof(event_subscriber_t));
    if (!subscriber) return 0;
    
    subscriber->event_name = event_strdup(event_name);
    if (!subscriber->event_name) {
        free(subscriber);
        return 0;
    }
    
    subscriber->event_type = EVENT_TYPE_CUSTOM;
    subscriber->handler = handler;
    subscriber->user_context = user_context;
    
    event_bus_lock(bus);
    
    subscriber->id = bus->next_subscription_id++;
    subscriber->next = bus->subscribers;
    bus->subscribers = subscriber;
    
    event_bus_unlock(bus);
    
    return subscriber->id;
}

event_subscription_t event_bus_subscribe_type(event_bus_t* bus, event_type_t event_type,
                                            event_handler_t handler, void* user_context) {
    if (!bus || !handler) return 0;
    
    event_subscriber_t* subscriber = (event_subscriber_t*)calloc(1, sizeof(event_subscriber_t));
    if (!subscriber) return 0;
    
    subscriber->event_type = event_type;
    subscriber->handler = handler;
    subscriber->user_context = user_context;
    
    event_bus_lock(bus);
    
    subscriber->id = bus->next_subscription_id++;
    subscriber->next = bus->subscribers;
    bus->subscribers = subscriber;
    
    event_bus_unlock(bus);
    
    return subscriber->id;
}

int event_bus_unsubscribe(event_bus_t* bus, event_subscription_t subscription) {
    if (!bus || subscription == 0) return 0;
    
    event_bus_lock(bus);
    
    event_subscriber_t* prev = NULL;
    event_subscriber_t* current = bus->subscribers;
    int found = 0;
    
    while (current) {
        if (current->id == subscription) {
            if (prev) {
                prev->next = current->next;
            } else {
                bus->subscribers = current->next;
            }
            
            free(current->event_name);
            free(current);
            found = 1;
            break;
        }
        
        prev = current;
        current = current->next;
    }
    
    event_bus_unlock(bus);
    return found;
}

void event_bus_unsubscribe_all(event_bus_t* bus, const char* event_name) {
    if (!bus) return;
    
    event_bus_lock(bus);
    
    event_subscriber_t* prev = NULL;
    event_subscriber_t* current = bus->subscribers;
    
    while (current) {
        if (!event_name || (current->event_name && strcmp(current->event_name, event_name) == 0)) {
            event_subscriber_t* to_remove = current;
            
            if (prev) {
                prev->next = current->next;
            } else {
                bus->subscribers = current->next;
            }
            
            current = current->next;
            
            free(to_remove->event_name);
            free(to_remove);
        } else {
            prev = current;
            current = current->next;
        }
    }
    
    event_bus_unlock(bus);
}

void event_bus_unsubscribe_all_type(event_bus_t* bus, event_type_t event_type) {
    if (!bus) return;
    
    event_bus_lock(bus);
    
    event_subscriber_t* prev = NULL;
    event_subscriber_t* current = bus->subscribers;
    
    while (current) {
        if (current->event_type == event_type) {
            event_subscriber_t* to_remove = current;
            
            if (prev) {
                prev->next = current->next;
            } else {
                bus->subscribers = current->next;
            }
            
            current = current->next;
            
            free(to_remove->event_name);
            free(to_remove);
        } else {
            prev = current;
            current = current->next;
        }
    }
    
    event_bus_unlock(bus);
}

static void event_bus_publish_to_subscribers(event_bus_t* bus, const event_t* event) {
    /* Create a copy of subscribers list to avoid modification during iteration */
    event_subscriber_t* subscribers_copy = NULL;
    event_subscriber_t* current = bus->subscribers;
    event_subscriber_t* copy_tail = NULL;
    
    while (current) {
        event_subscriber_t* copy = (event_subscriber_t*)malloc(sizeof(event_subscriber_t));
        if (!copy) break;
        
        memcpy(copy, current, sizeof(event_subscriber_t));
        copy->next = NULL;
        copy->event_name = current->event_name ? event_strdup(current->event_name) : NULL;
        
        if (!subscribers_copy) {
            subscribers_copy = copy;
            copy_tail = copy;
        } else {
            copy_tail->next = copy;
            copy_tail = copy;
        }
        
        current = current->next;
    }
    
    /* Call handlers */
    current = subscribers_copy;
    while (current) {
        int match = 0;
        
        if (event->name && current->event_name) {
            match = (strcmp(event->name, current->event_name) == 0);
        } else if (event->type != EVENT_TYPE_CUSTOM && current->event_type != EVENT_TYPE_CUSTOM) {
            match = (event->type == current->event_type);
        }
        
        if (match) {
            current->handler(event);
        }
        
        event_subscriber_t* next = current->next;
        free(current->event_name);
        free(current);
        current = next;
    }
}

int event_bus_publish(event_bus_t* bus, const char* event_name, void* data, 
                     size_t data_size, void* user_context) {
    if (!bus || !event_name) return 0;
    
    event_t event;
    event.type = EVENT_TYPE_CUSTOM;
    event.name = event_name;
    event.data = data;
    event.data_size = data_size;
    event.user_context = user_context;
    
    event_bus_lock(bus);
    event_bus_publish_to_subscribers(bus, &event);
    event_bus_unlock(bus);
    
    return 1;
}

int event_bus_publish_type(event_bus_t* bus, event_type_t event_type, void* data,
                          size_t data_size, void* user_context) {
    if (!bus) return 0;
    
    event_t event;
    event.type = event_type;
    event.name = NULL;
    event.data = data;
    event.data_size = data_size;
    event.user_context = user_context;
    
    event_bus_lock(bus);
    event_bus_publish_to_subscribers(bus, &event);
    event_bus_unlock(bus);
    
    return 1;
}

event_t* event_create(event_type_t type, const char* name, void* data, 
                     size_t data_size, void* user_context) {
    event_t* event = (event_t*)calloc(1, sizeof(event_t));
    if (!event) return NULL;
    
    event->type = type;
    event->name = name ? event_strdup(name) : NULL;
    event->data = data;
    event->data_size = data_size;
    event->user_context = user_context;
    
    return event;
}

void event_destroy(event_t* event) {
    if (event) {
        free(event->name);
        free(event);
    }
}

size_t event_bus_subscriber_count(const event_bus_t* bus, const char* event_name) {
    if (!bus) return 0;
    
    size_t count = 0;
    const event_subscriber_t* current = bus->subscribers;
    
    while (current) {
        if (current->event_name && 
            (!event_name || strcmp(current->event_name, event_name) == 0)) {
            count++;
        }
        current = current->next;
    }
    
    return count;
}

size_t event_bus_subscriber_count_type(const event_bus_t* bus, event_type_t event_type) {
    if (!bus) return 0;
    
    size_t count = 0;
    const event_subscriber_t* current = bus->subscribers;
    
    while (current) {
        if (current->event_type == event_type) {
            count++;
        }
        current = current->next;
    }
    
    return count;
}

void event_bus_set_global_context(event_bus_t* bus, void* global_context) {
    if (bus) {
        bus->global_context = global_context;
    }
}

void* event_bus_get_global_context(const event_bus_t* bus) {
    return bus ? bus->global_context : NULL;
}

void* event_data_alloc(size_t size) {
    return malloc(size);
}

void event_data_free(void* data) {
    free(data);
}

#endif /* EVENTS_IMPLEMENTATION */
