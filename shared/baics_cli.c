/**
 * Command-line argument parser template
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_usage(const char *program_name) {
  printf("Usage: %s [options]\n", program_name);
  printf("Options:\n");
  printf("  -h, --help     Show this help message\n");
  printf("  -v, --version  Show version information\n");
  printf("  -f <file>      Specify input file\n");
  printf("  -o <file>      Specify output file\n");
}

int main(int argc, char *argv[]) {
  char *input_file = NULL;
  char *output_file = NULL;

  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
      print_usage(argv[0]);
      return 0;
    } else if (strcmp(argv[i], "-v") == 0 ||
               strcmp(argv[i], "--version") == 0) {
      printf("Program Version 1.0\n");
      return 0;
    } else if (strcmp(argv[i], "-f") == 0) {
      if (i + 1 < argc) {
        input_file = argv[++i];
      } else {
        fprintf(stderr, "Error: -f requires a filename\n");
        return 1;
      }
    } else if (strcmp(argv[i], "-o") == 0) {
      if (i + 1 < argc) {
        output_file = argv[++i];
      } else {
        fprintf(stderr, "Error: -o requires a filename\n");
        return 1;
      }
    } else {
      fprintf(stderr, "Error: Unknown option '%s'\n", argv[i]);
      print_usage(argv[0]);
      return 1;
    }
  }

  printf("Input file: %s\n", input_file ? input_file : "None");
  printf("Output file: %s\n", output_file ? output_file : "None");

  return 0;
}
