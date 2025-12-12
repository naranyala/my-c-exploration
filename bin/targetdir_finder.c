#define _CRT_SECURE_NO_WARNINGS
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <direct.h>
#include <windows.h>
#define GETCWD _getcwd
#define NATIVE_SEP '\\'
#else
#define _POSIX_C_SOURCE 200809L
#include <dirent.h>
#include <libgen.h>
#include <sys/stat.h>
#include <unistd.h>
#define GETCWD getcwd
#define NATIVE_SEP '/'
#endif

#define PATH_BUF_SIZE 4096

// Output style
typedef enum { STYLE_NATIVE, STYLE_WIN, STYLE_UNIX } path_style_t;

static path_style_t g_output_style = STYLE_NATIVE;

// Simple basename
char *simple_basename(char *path) {
  char *p = path;
  char *last = p;
  while (*p) {
    if (*p == '/' || *p == '\\')
      last = p + 1;
    p++;
  }
  return last[0] ? last : p;
}

void print_help(char *prog) {
  char *name = simple_basename(prog);
  printf("Usage: %s [OPTIONS] [START_PATH]\n", name);
  printf("Find all directories named 'target' recursively.\n");
  printf("Default START_PATH: current directory\n\n");
  printf("Options:\n");
  printf("  -h, --help        Show this help\n");
  printf("  --win-slash       Output paths with backslashes (\\\\)\n");
  printf("  --unix-slash     Output paths with forward slashes (/)\n");
  printf("\nExamples:\n");
  printf("  %s ../../projects\n", name);
  printf("  %s --win-slash /home\n", name);
  printf("  %s --unix-slash C:\\\\Users\n", name);
}

// Normalize path separator for output
void print_normalized_path(const char *path) {
  if (g_output_style == STYLE_NATIVE) {
    puts(path);
    return;
  }

  char normalized[PATH_BUF_SIZE];
  size_t i = 0, j = 0;
  char target_sep = (g_output_style == STYLE_WIN) ? '\\' : '/';

  while (path[i] && j < sizeof(normalized) - 1) {
    if (path[i] == '/' || path[i] == '\\') {
      normalized[j++] = target_sep;
    } else {
      normalized[j++] = path[i];
    }
    i++;
  }
  normalized[j] = '\0';
  puts(normalized);
}

#ifdef _WIN32

int canonicalize_path(const char *input, char *output, size_t out_size) {
  // Use GetFullPathName to resolve .., ., and get absolute path
  DWORD len = GetFullPathNameA(input, (DWORD)out_size, output, NULL);
  if (len == 0 || len >= out_size) {
    fprintf(stderr, "Error: path too long or invalid: %s\n", input);
    return -1;
  }

  // Validate it's a directory
  DWORD attr = GetFileAttributesA(output);
  if (attr == INVALID_FILE_ATTRIBUTES) {
    fprintf(stderr, "Error: cannot access '%s': %lu\n", output, GetLastError());
    return -1;
  }
  if (!(attr & FILE_ATTRIBUTE_DIRECTORY)) {
    fprintf(stderr, "Error: not a directory: %s\n", output);
    return -1;
  }
  return 0;
}

void find_target_dirs(const char *current_path) {
  char search_path[PATH_BUF_SIZE];
  char full_path[PATH_BUF_SIZE];
  int len = snprintf(search_path, sizeof(search_path), "%s\\*", current_path);
  if (len >= (int)sizeof(search_path))
    return;

  WIN32_FIND_DATAA fd;
  HANDLE h = FindFirstFileA(search_path, &fd);
  if (h == INVALID_HANDLE_VALUE)
    return;

  do {
    if (strcmp(fd.cFileName, ".") == 0 || strcmp(fd.cFileName, "..") == 0)
      continue;

    len = snprintf(full_path, sizeof(full_path), "%s\\%s", current_path,
                   fd.cFileName);
    if (len >= (int)sizeof(full_path))
      continue;

    if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
      if (_stricmp(fd.cFileName, "target") == 0) {
        print_normalized_path(full_path);
      } else {
        find_target_dirs(full_path);
      }
    }
  } while (FindNextFileA(h, &fd));
  FindClose(h);
}

#else // POSIX

int canonicalize_path(const char *input, char *output, size_t out_size) {
  // realpath resolves .., ., symlinks, and gives absolute path
  char *real = realpath(input, NULL);
  if (!real) {
    fprintf(stderr, "Error: cannot resolve path '%s': %s\n", input,
            strerror(errno));
    return -1;
  }

  if (strlen(real) >= out_size) {
    free(real);
    fprintf(stderr, "Error: resolved path too long\n");
    return -1;
  }

  strcpy(output, real);
  free(real);

  struct stat st;
  if (stat(output, &st) != 0) {
    fprintf(stderr, "Error: cannot access '%s': %s\n", output, strerror(errno));
    return -1;
  }
  if (!S_ISDIR(st.st_mode)) {
    fprintf(stderr, "Error: not a directory: %s\n", output);
    return -1;
  }
  return 0;
}

void find_target_dirs(const char *current_path) {
  DIR *dir = opendir(current_path);
  if (!dir)
    return;

  struct dirent *entry;
  while ((entry = readdir(dir)) != NULL) {
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
      continue;

    char full_path[PATH_BUF_SIZE];
    int len = snprintf(full_path, sizeof(full_path), "%s/%s", current_path,
                       entry->d_name);
    if (len >= (int)sizeof(full_path))
      continue;

    struct stat st;
    if (stat(full_path, &st) == 0 && S_ISDIR(st.st_mode)) {
      if (strcmp(entry->d_name, "target") == 0) {
        print_normalized_path(full_path);
      } else {
        find_target_dirs(full_path);
      }
    }
  }
  closedir(dir);
}

#endif

int main(int argc, char *argv[]) {
  const char *start_input = NULL;
  int i = 1;

  // Parse options
  while (i < argc) {
    if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
      print_help(argv[0]);
      return 0;
    } else if (strcmp(argv[i], "--win-slash") == 0) {
      g_output_style = STYLE_WIN;
      i++;
    } else if (strcmp(argv[i], "--unix-slash") == 0) {
      g_output_style = STYLE_UNIX;
      i++;
    } else if (argv[i][0] == '-') {
      fprintf(stderr, "Unknown option: %s\n", argv[i]);
      fprintf(stderr, "Use --help for usage\n");
      return 1;
    } else {
      // Found start path
      start_input = argv[i];
      i++;
      break;
    }
  }

  // Only one positional argument allowed
  if (i < argc) {
    fprintf(stderr, "Error: too many arguments\n");
    fprintf(stderr, "Use --help for usage\n");
    return 1;
  }

  char start_abs[PATH_BUF_SIZE];
  const char *use_path = start_input ? start_input : ".";

  if (canonicalize_path(use_path, start_abs, sizeof(start_abs)) != 0) {
    return 1;
  }

  find_target_dirs(start_abs);
  return 0;
}
