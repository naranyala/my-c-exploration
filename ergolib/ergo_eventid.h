#ifndef ERGO_EVENTID_H
#define ERGO_EVENTID_H

#include <stdatomic.h>
#include <stdint.h>

static atomic_uint_fast64_t ergo_event_counter = 1;

static inline uint64_t ergo_event_id(void) {
    return atomic_fetch_add(&ergo_event_counter, 1);
}

#endif

