#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

static void usage(const char *prog) {
  fprintf(stderr, "Usage: %s [-h] [-v] [-o outfile] [-n count] file...\n",
          prog);
  exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
  const char *outfile = NULL;
  int verbose = 0;
  int count = 10;

  int opt;
  while ((opt = getopt(argc, argv, "hvo:n:")) != -1) {
    switch (opt) {
    case 'h':
      usage(argv[0]);
      break;
    case 'v':
      verbose = 1;
      break;
    case 'o':
      outfile = optarg;
      break;
    case 'n':
      count = atoi(optarg);
      break;
    default:
      usage(argv[0]);
      break;
    }
  }

  if (optind >= argc) {
    fprintf(stderr, "Expected argument after options\n");
    usage(argv[0]);
  }

  // remaining args: argv[optind] ... argv[argc-1]

  return EXIT_SUCCESS;
}
