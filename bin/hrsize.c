#include <dirent.h>
#include <errno.h> // Added for better error handling
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

// --- Configuration Structure ---
typedef struct {
  int show_help;     // Flag for -h/--help
  int summarize;     // Flag for -s/--summarize (fixed name to match usage)
  int apparent_size; // Flag for -a/--apparent-size (default)
  int block_size;    // Flag for -b/--bytes (disk usage)
} Config;

// --- Function Prototypes ---
void print_usage(const char *prog_name);
off_t get_dir_size(const char *path, const Config *config);
void format_bytes(off_t bytes, char *buf, size_t bufsize);
void process_path(const char *path, const Config *config, int is_top_level);

int main(int argc, char *argv[]) {
  Config config = {0, 0, 1, 0}; // Default: show apparent size, not summarize

  // --- Argument Parsing using getopt_long ---
  static struct option long_options[] = {{"help", no_argument, 0, 'h'},
                                         {"summarize", no_argument, 0, 's'},
                                         {"apparent-size", no_argument, 0, 'a'},
                                         {"bytes", no_argument, 0, 'b'},
                                         {0, 0, 0, 0}};

  int option_index = 0;
  int c;

  while ((c = getopt_long(argc, argv, "hsab", long_options, &option_index)) !=
         -1) {
    switch (c) {
    case 'h':
      config.show_help = 1;
      break;
    case 's':
      config.summarize = 1;
      break;
    case 'a':
      config.apparent_size = 1;
      config.block_size = 0;
      break;
    case 'b':
      config.block_size = 1;
      config.apparent_size = 0;
      break;
    case '?': // Invalid option
      print_usage(argv[0]);
      return 1;
    default:
      abort();
    }
  }

  if (config.show_help) {
    print_usage(argv[0]);
    return 0;
  }

  // After parsing options, the remaining arguments are file/directory paths.
  // optind is the index of the next argument to be processed.
  if (optind >= argc) {
    fprintf(stderr, "Error: No paths specified.\n");
    print_usage(argv[0]);
    return 1;
  }

  // --- Process Paths ---
  for (int i = optind; i < argc; i++) {
    process_path(argv[i], &config, 1);
  }

  return 0;
}

/**
 * @brief Prints a usage/help message to stderr.
 */
void print_usage(const char *prog_name) {
  fprintf(stderr, "Usage: %s [OPTIONS] <file1> [file2] [directory1] ...\n",
          prog_name);
  fprintf(stderr, "Calculate and display human-readable sizes of files and "
                  "directories.\n\n");
  fprintf(stderr, "Options:\n");
  fprintf(stderr, "  -h, --help          Show this help message and exit.\n");
  fprintf(stderr,
          "  -s, --summarize     Display only a total for each argument.\n");
  fprintf(stderr, "  -a, --apparent-size Print apparent sizes (default).\n");
  fprintf(stderr,
          "  -b, --bytes         Print actual disk usage (block size).\n");
}

/**
 * @brief Processes a single path (file or directory) and prints its size.
 * @param path The path to process.
 * @param config The program configuration.
 * @param is_top_level Flag to control behavior for summarize mode.
 */
void process_path(const char *path, const Config *config, int is_top_level) {
  struct stat sb;
  if (lstat(path, &sb) == -1) {
    perror(path);
    return;
  }

  off_t size = 0;
  if (S_ISREG(sb.st_mode) || S_ISLNK(sb.st_mode)) {
    // For files and symlinks, size is straightforward
    size = (config->apparent_size) ? sb.st_size : sb.st_blocks * 512;
  } else if (S_ISDIR(sb.st_mode)) {
    // For directories, we need to recurse
    size = get_dir_size(path, config);
  } else {
    // Ignore other file types (sockets, devices, etc.)
    return;
  }

  char human_readable[16];
  format_bytes(size, human_readable, sizeof(human_readable));

  // Only print for top-level items or when not in summarize mode
  if (is_top_level || !config->summarize) {
    printf("%s\t%s\n", human_readable, path);
  }

  // If not in summarize mode and it's a directory, recurse to print sub-items
  if (!config->summarize && S_ISDIR(sb.st_mode)) {
    DIR *dir = opendir(path);
    if (dir == NULL) {
      perror(path);
      return;
    }

    struct dirent *entry;
    char *full_path = NULL;

    while ((entry = readdir(dir)) != NULL) {
      if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
        continue;
      }

      // Allocate enough space for the full path
      size_t path_len = strlen(path) + strlen(entry->d_name) + 2;
      full_path = malloc(path_len);
      if (full_path == NULL) {
        perror("malloc");
        break;
      }

      snprintf(full_path, path_len, "%s/%s", path, entry->d_name);

      // Recursive call for sub-items
      process_path(full_path, config,
                   0); // 0 indicates this is not a top-level argument

      free(full_path);
      full_path = NULL;
    }

    if (full_path) {
      free(full_path);
    }
    closedir(dir);
  }
}

/**
 * @brief Recursively calculates the total size of a directory.
 * @param path The path to the directory.
 * @param config The program configuration.
 * @return The total size in bytes, or -1 on error.
 */
off_t get_dir_size(const char *path, const Config *config) {
  DIR *dir = opendir(path);
  if (dir == NULL) {
    // This can happen for directories we can't read, but we still want to
    // count the directory entry itself.
    return 0;
  }

  off_t total_size = 0;
  struct dirent *entry;

  while ((entry = readdir(dir)) != NULL) {
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
      continue;
    }

    // Allocate enough space for the full path
    size_t path_len = strlen(path) + strlen(entry->d_name) + 2;
    char *full_path = malloc(path_len);
    if (full_path == NULL) {
      perror("malloc");
      closedir(dir);
      return total_size;
    }

    snprintf(full_path, path_len, "%s/%s", path, entry->d_name);

    struct stat entry_sb;
    if (lstat(full_path, &entry_sb) == -1) {
      // Can't stat, skip it.
      free(full_path);
      continue;
    }

    if (S_ISDIR(entry_sb.st_mode)) {
      total_size += get_dir_size(full_path, config);
    } else {
      total_size +=
          (config->apparent_size) ? entry_sb.st_size : entry_sb.st_blocks * 512;
    }

    free(full_path);
  }

  closedir(dir);
  return total_size;
}

/**
 * @brief Formats a byte count into a human-readable string (e.g., "1.2M").
 * @param bytes The number of bytes.
 * @param buf The character buffer to store the result.
 * @param bufsize The size of the buffer.
 */
void format_bytes(off_t bytes, char *buf, size_t bufsize) {
  const char *units[] = {"B", "K", "M", "G", "T", "P"};
  const int base = 1024;
  int i = 0;
  double size = (double)bytes;

  if (bytes < base) {
    snprintf(buf, bufsize, "%lld%s", (long long)bytes, units[i]);
    return;
  }

  while (size >= base && i < (int)(sizeof(units) / sizeof(units[0])) - 1) {
    size /= base;
    i++;
  }

  snprintf(buf, bufsize, "%.1f%s", size, units[i]);
}
