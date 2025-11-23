
/* Usage */
#include "ergo_args.h"

int main(int argc, char **argv) {
    args_t a = args_init(argc, argv);
    if (args_flag(&a, "-h") || args_flag(&a, "--help")) args_help(argv[0], "My awesome tool");
    int verbose = args_flag(&a, "-v");
    const char *output = args_str(&a, "-o", "out.txt");
    int count = args_int(&a, "-n", 10);

    while (!args_end(a)) {
        const char *file = args_positional(&a);
        if (file) printf("Input: %s\n", file);
    }
}
