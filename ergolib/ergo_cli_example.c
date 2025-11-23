
#include "ergo_cli.h"

int main(int argc, char **argv) {
    if (ergo_cli_has(argc, argv, "--help")) {
        printf("Usage: demo [--name NAME]\n");
    }
    const char *name = ergo_cli_get(argc, argv, "--name");
    printf("Hello %s!\n", name ? name : "World");
}
