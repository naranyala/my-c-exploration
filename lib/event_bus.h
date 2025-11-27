#ifndef EVENT_BUS_H
#define EVENT_BUS_H

#ifdef __cplusplus
extern "C" {
#endif

/* ---------------------------------------------------------
   Configuration
   --------------------------------------------------------- */
#ifndef EVENTBUS_MAX_LISTENERS
#define EVENTBUS_MAX_LISTENERS 64
#endif

typedef void (*eventbus_callback)(const char* event, void* data);

/* ---------------------------------------------------------
   Storage
   --------------------------------------------------------- */
typedef struct {
    const char* event;
    eventbus_callback cb;
} eventbus_entry;

static eventbus_entry eventbus_table[EVENTBUS_MAX_LISTENERS];
static int eventbus_count = 0;

/* ---------------------------------------------------------
   Register listener
   --------------------------------------------------------- */
static int eventbus_on(const char* event, eventbus_callback cb) {
    if (eventbus_count >= EVENTBUS_MAX_LISTENERS)
        return -1;

    eventbus_table[eventbus_count++] = (eventbus_entry){event, cb};
    return 0;
}

/* ---------------------------------------------------------
   Remove listener
   --------------------------------------------------------- */
static int eventbus_off(const char* event, eventbus_callback cb) {
    for (int i = 0; i < eventbus_count; i++) {
        if (eventbus_table[i].event == event &&
            eventbus_table[i].cb == cb)
        {
            eventbus_table[i] = eventbus_table[eventbus_count - 1];
            eventbus_count--;
            return 0;
        }
    }
    return -1;
}

/* ---------------------------------------------------------
   Emit event
   --------------------------------------------------------- */
static void eventbus_emit(const char* event, void* data) {
    for (int i = 0; i < eventbus_count; i++) {
        if (eventbus_table[i].event == event)
            eventbus_table[i].cb(event, data);
    }
}

#ifdef __cplusplus
}
#endif

#endif /* EVENT_BUS_H */

