#include <ctype.h>
#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SEPARATOR "---"
#define PROGRAM_NAME "headonly_md_parser"
#define VERSION "1.0"
#define DEFAULT_SEPARATOR_COUNT 2

enum {
  EXIT_OK = 0,
  EXIT_BAD_ARGS = 1,
  EXIT_BAD_FILE = 2,
  EXIT_BAD_MEM = 3,
  EXIT_BAD_PROCESS = 4
};

void print_usage(const char *prog_name);
void print_version(void);
void trim_whitespace(char *str);
int is_ignored_element(const char *line);
int is_heading(const char *line);
int process_file(FILE *input, FILE *output, int break_after_separators);
void handle_error(const char *message, int exit_code);

void print_usage(const char *prog_name) {
  fprintf(stderr, "Usage: %s [OPTIONS] [FILE]\n", prog_name);
  fprintf(stderr, "Extract content between separators in Markdown files.\n\n");
  fprintf(stderr, "Options:\n");
  fprintf(stderr,
          "  -h, --help           Display this help message and exit\n");
  fprintf(stderr,
          "  -V, --version        Display version information and exit\n");
  fprintf(stderr,
          "  -o, --output FILE    Write output to FILE instead of stdout\n");
  fprintf(stderr,
          "  -i, --input FILE     Read input from FILE instead of stdin\n");
  fprintf(stderr,
          "  -s, --separators N   Stop after N separators (default: 2)\n");
  fprintf(stderr,
          "\nIf no input file is specified, reads from standard input.\n");
  fprintf(stderr,
          "If no output file is specified, writes to standard output.\n");
}

void print_version(void) { printf("%s version %s\n", PROGRAM_NAME, VERSION); }

void handle_error(const char *message, int exit_code) {
  if (errno != 0) {
    fprintf(stderr, "Error: %s: %s\n", message, strerror(errno));
  } else {
    fprintf(stderr, "Error: %s\n", message);
  }
  exit(exit_code);
}

void trim_whitespace(char *str) {
  if (str == NULL || *str == '\0')
    return;

  char *start = str;
  while (isspace((unsigned char)*start))
    start++;

  if (*start == '\0') {
    str[0] = '\0';
    return;
  }

  char *end = start + strlen(start) - 1;
  while (end > start && isspace((unsigned char)*end))
    end--;

  if (start != str) {
    memmove(str, start, end - start + 1);
  }
  str[end - start + 1] = '\0';
}

int is_heading(const char *line) {
  if (line == NULL || *line == '\0')
    return 0;

  // Check for ATX-style headings (#, ##, ###, etc.)
  if (line[0] == '#') {
    int i = 1;
    while (line[i] == '#')
      i++;
    if (i <= 6 && (line[i] == ' ' || line[i] == '\t')) {
      return 1;
    }
  }

  // Check for Setext-style headings (=== or --- underlining)
  // This is harder to detect without context of previous line
  // We'll handle this in the main processing logic

  return 0;
}

int is_ignored_element(const char *line) {
  if (line == NULL || *line == '\0')
    return 0;

  // Headings are ignored
  if (is_heading(line))
    return 1;

  // HTML comments
  if (strncmp(line, "<!--", 4) == 0)
    return 1;

  // Blockquotes
  if (line[0] == '>')
    return 1;

  // Horizontal rules (---, ***, ___)
  if ((strcmp(line, "---") == 0 || strcmp(line, "***") == 0 ||
       strcmp(line, "___") == 0)) {
    return 1;
  }

  // Unordered list items
  if ((line[0] == '-' || line[0] == '*' || line[0] == '+') &&
      isspace((unsigned char)line[1])) {
    return 1;
  }

  // Ordered list items
  if (isdigit((unsigned char)line[0])) {
    const char *p = line + 1;
    while (isdigit((unsigned char)*p))
      p++;
    if ((p[0] == '.' || p[0] == ')') && isspace((unsigned char)p[1])) {
      return 1;
    }
  }

  // Code blocks (starting with ``` or ~~~)
  if (strncmp(line, "```", 3) == 0 || strncmp(line, "~~~", 3) == 0) {
    return 1;
  }

  return 0;
}

int process_file(FILE *input, FILE *output, int break_after_separators) {
  if (input == NULL || output == NULL)
    return EXIT_BAD_ARGS;

  char *line = NULL;
  size_t len = 0;
  ssize_t read;
  int separators_found = 0;
  int in_paragraph = 0;
  int success = 0;
  int start_processing_after = (break_after_separators > 0) ? 1 : 0;
  char *prev_line = NULL;
  size_t prev_len = 0;

  while ((read = getline(&line, &len, input)) != -1) {
    char *trimmed_line = strdup(line);
    if (trimmed_line == NULL) {
      handle_error("Failed to allocate memory", EXIT_BAD_MEM);
    }
    trim_whitespace(trimmed_line);

    // Check for separator
    if (strcmp(trimmed_line, SEPARATOR) == 0) {
      separators_found++;
      if (separators_found >= break_after_separators) {
        free(trimmed_line);
        success = 1;
        break;
      }
      free(trimmed_line);
      continue;
    }

    // Check for Setext-style heading (=== or --- underlining)
    int is_setext_heading = 0;
    if (prev_line != NULL && strlen(prev_line) > 0) {
      char *prev_trimmed = strdup(prev_line);
      if (prev_trimmed) {
        trim_whitespace(prev_trimmed);
        if (strlen(prev_trimmed) > 0 && (strcmp(trimmed_line, "===") == 0 ||
                                         strcmp(trimmed_line, "---") == 0)) {
          is_setext_heading = 1;
          // Also mark previous line as heading
          free(prev_trimmed);
          free(trimmed_line);
          free(prev_line);
          prev_line = NULL;
          continue;
        }
        free(prev_trimmed);
      }
    }

    if (separators_found >= start_processing_after) {
      int is_blank = (strlen(trimmed_line) == 0);

      if (is_blank) {
        if (in_paragraph) {
          fprintf(output, "\n");
          in_paragraph = 0;
        }
      } else if (is_setext_heading || is_ignored_element(trimmed_line)) {
        if (in_paragraph) {
          fprintf(output, "\n");
          in_paragraph = 0;
        }
        // Skip this line
        free(trimmed_line);

        // Store previous line for Setext heading detection
        if (prev_line)
          free(prev_line);
        prev_line = strdup(line);
        continue;
      } else {
        // Regular paragraph content
        if (!in_paragraph)
          in_paragraph = 1;
        fprintf(output, "%s", line);
      }
    }

    // Store previous line for Setext heading detection
    if (prev_line)
      free(prev_line);
    prev_line = strdup(line);

    free(trimmed_line);
  }

  if (read == -1 && !feof(input)) {
    if (line)
      free(line);
    if (prev_line)
      free(prev_line);
    handle_error("Error reading input", EXIT_BAD_FILE);
  }

  // End last paragraph if needed
  if (in_paragraph) {
    fprintf(output, "\n");
  }

  if (separators_found < break_after_separators && separators_found > 0) {
    fprintf(stderr, "Warning: Found only %d separator(s), expected %d\n",
            separators_found, break_after_separators);
  } else if (separators_found == 0 && break_after_separators > 0) {
    fprintf(stderr, "Warning: No separators found in input\n");
  }

  if (line)
    free(line);
  if (prev_line)
    free(prev_line);

  return (break_after_separators == 0 || success) ? EXIT_OK : EXIT_BAD_PROCESS;
}

int main(int argc, char *argv[]) {
  FILE *input = stdin;
  FILE *output = stdout;
  char *input_filename = NULL;
  char *output_filename = NULL;
  int separator_count = DEFAULT_SEPARATOR_COUNT;
  int help_requested = 0;
  int version_requested = 0;
  int close_input = 0;
  int close_output = 0;
  int exit_code = EXIT_OK;

  static struct option long_options[] = {
      {"help", no_argument, 0, 'h'},
      {"version", no_argument, 0, 'V'},
      {"output", required_argument, 0, 'o'},
      {"input", required_argument, 0, 'i'},
      {"separators", required_argument, 0, 's'},
      {0, 0, 0, 0}};

  int opt;
  int option_index = 0;

  while ((opt = getopt_long(argc, argv, "hVo:i:s:", long_options,
                            &option_index)) != -1) {
    switch (opt) {
    case 'h':
      help_requested = 1;
      break;
    case 'V':
      version_requested = 1;
      break;
    case 'i':
      input_filename = optarg;
      break;
    case 'o':
      output_filename = optarg;
      break;
    case 's':
      separator_count = atoi(optarg);
      if (separator_count < 0) {
        fprintf(stderr, "Error: Separator count must be non-negative\n");
        return EXIT_BAD_ARGS;
      }
      break;
    case '?':
      print_usage(argv[0]);
      return EXIT_BAD_ARGS;
    default:
      abort();
    }
  }

  if (help_requested) {
    print_usage(argv[0]);
    return EXIT_OK;
  }

  if (version_requested) {
    print_version();
    return EXIT_OK;
  }

  if (optind < argc) {
    if (input_filename != NULL) {
      fprintf(stderr,
              "Warning: Input file specified multiple times. Using: %s\n",
              argv[optind]);
    }
    input_filename = argv[optind];
  }

  if (input_filename == NULL && optind >= argc) {
    fprintf(stderr, "Error: No input specified\n");
    fprintf(stderr, "Use -h for help\n");
    return EXIT_BAD_ARGS;
  }

  if (input_filename != NULL) {
    input = fopen(input_filename, "r");
    if (input == NULL)
      handle_error("Failed to open input file", EXIT_BAD_FILE);
    close_input = 1;
  }

  if (output_filename != NULL) {
    output = fopen(output_filename, "w");
    if (output == NULL) {
      if (close_input)
        fclose(input);
      handle_error("Failed to open output file", EXIT_BAD_FILE);
    }
    close_output = 1;
  }

  exit_code = process_file(input, output, separator_count);

  if (close_input && input != NULL && fclose(input) != 0) {
    fprintf(stderr, "Warning: Failed to close input file\n");
  }

  if (close_output && output != NULL && fclose(output) != 0) {
    fprintf(stderr, "Warning: Failed to close output file\n");
  }

  return exit_code;
}
