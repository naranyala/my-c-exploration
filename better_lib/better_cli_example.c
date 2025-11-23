
#include "better_cli.h"

int main(int argc, char **argv) {
    const char *name = cli_getopt(argc, argv, "--name");
    if (cli_hasflag(argc, argv, "--verbose")) {
        printf("Verbose mode ON\n");
    }
    printf("Hello %s!\n", name ? name : "World");
}
