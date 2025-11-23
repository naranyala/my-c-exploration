#define BETTER_ARGS_IMPLEMENTATION
#include "better_args.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    ba_init(argc, argv);

    if (ba_has("--help")) {
        printf("Usage: app --port <n> --verbose\n");
        return 0;
    }

    bool verbose = ba_has("--verbose");
    const char* port_str = ba_val("--port", "8080");
    int port = atoi(port_str);

    if (verbose) printf("Starting server on port %d...\n", port);
    return 0;
}
