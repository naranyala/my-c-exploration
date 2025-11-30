
#include <stdio.h>
#include <string.h>

typedef struct {
    int verbose;
    int help;
    char *inputFile;
    char *outputFile;
} Arguments;

// Parse command-line arguments
void parseArguments(int argc, char *argv[], Arguments *args) {
    args->verbose = 0;
    args->help = 0;
    args->inputFile = NULL;
    args->outputFile = NULL;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0) {
            args->verbose = 1;
        } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            args->help = 1;
        } else if (strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--input") == 0) {
            if (i + 1 < argc) {
                args->inputFile = argv[++i];
            }
        } else if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output") == 0) {
            if (i + 1 < argc) {
                args->outputFile = argv[++i];
            }
        }
    }
}
