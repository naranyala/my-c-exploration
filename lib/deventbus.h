/* deventbus.h -- Damn-Stupid Event Bus for C
 * MIT License
 * Usage: #define DEVENTBUS_IMPL before including in one .c file
 */

#ifndef DEVENTBUS_H
#define DEVENTBUS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h> // for size_t
#include <string.h> // for strcmp

// ----------------------------
// CONFIGURATION (tweak these!)
// ----------------------------

#ifndef DEVENTBUS_MAX_SUBSCRIBERS
#define DEVENTBUS_MAX_SUBSCRIBERS 32
#endif

#ifndef DEVENTBUS_MAX_EVENT_NAME_LEN
#define DEVENTBUS_MAX_EVENT_NAME_LEN 32
#endif

// ----------------------------
// PUBLIC API
// ----------------------------

// Callback signature: receives event name and user-provided payload
typedef void (*deventbus_handler_t)(const char *event, void *payload);

// Subscribe a handler to an event
// Returns 0 on success, -1 if too many subscribers
int deventbus_subscribe(const char *event, deventbus_handler_t handler);

// Publish an event with optional payload
// Calls all subscribed handlers
void deventbus_publish(const char *event, void *payload);

// Unsubscribe a handler from an event
// Returns 0 on success, -1 if not found
int deventbus_unsubscribe(const char *event, deventbus_handler_t handler);

#ifdef __cplusplus
}
#endif

#endif // DEVENTBUS_H

// ----------------------------
// IMPLEMENTATION
// ----------------------------

#ifdef DEVENTBUS_IMPL

typedef struct {
  char event[DEVENTBUS_MAX_EVENT_NAME_LEN];
  deventbus_handler_t handler;
} deventbus_sub_t;

static deventbus_sub_t _subs[DEVENTBUS_MAX_SUBSCRIBERS] = {0};
static size_t _sub_count = 0;

int deventbus_subscribe(const char *event, deventbus_handler_t handler) {
  if (!_subs || !event || !handler)
    return -1;
  if (_sub_count >= DEVENTBUS_MAX_SUBSCRIBERS)
    return -1;

  if (strlen(event) >= DEVENTBUS_MAX_EVENT_NAME_LEN)
    return -1;

  // Prevent duplicate subscriptions
  for (size_t i = 0; i < _sub_count; ++i) {
    if (_subs[i].handler == handler && strcmp(_subs[i].event, event) == 0) {
      return 0; // already subscribed
    }
  }

  strcpy(_subs[_sub_count].event, event);
  _subs[_sub_count].handler = handler;
  _sub_count++;
  return 0;
}

void deventbus_publish(const char *event, void *payload) {
  if (!event)
    return;
  for (size_t i = 0; i < _sub_count; ++i) {
    if (strcmp(_subs[i].event, event) == 0) {
      _subs[i].handler(event, payload);
    }
  }
}

int deventbus_unsubscribe(const char *event, deventbus_handler_t handler) {
  if (!event || !handler)
    return -1;
  for (size_t i = 0; i < _sub_count; ++i) {
    if (_subs[i].handler == handler && strcmp(_subs[i].event, event) == 0) {
      // Shift remaining entries left
      for (size_t j = i; j < _sub_count - 1; ++j) {
        _subs[j] = _subs[j + 1];
      }
      _sub_count--;
      return 0;
    }
  }
  return -1; // not found
}

#endif // DEVENTBUS_IMPL
