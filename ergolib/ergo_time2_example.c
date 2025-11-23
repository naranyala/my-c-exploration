#include "ergo_time2.h"

int main() {
    char ts[32];
    ergo_time_iso8601(ts, sizeof(ts));
    printf("Current UTC time: %s\n", ts);

    printf("Sleeping 500ms...\n");
    ergo_sleep_ms(500);
}

