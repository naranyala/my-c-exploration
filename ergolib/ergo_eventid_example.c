#include "ergo_eventid.h"
#include <stdio.h>

int main() {
    printf("%llu\n", (unsigned long long)ergo_event_id());
    printf("%llu\n", (unsigned long long)ergo_event_id());
}

