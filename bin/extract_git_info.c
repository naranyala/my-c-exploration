#define _POSIX_C_SOURCE 200809L
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
// Windows doesn't have getline/getdelim natively
static ssize_t getline(char **lineptr, size_t *n, FILE *stream) {
  size_t pos = 0;
  int c;
  if (lineptr == NULL || n == NULL || stream == NULL) {
    errno = EINVAL;
    return -1;
  }
  if (*lineptr == NULL) {
    *n = 128;
    *lineptr = malloc(*n);
    if (*lineptr == NULL) return -1;
  }
  while ((c = fgetc(stream)) != EOF) {
    if (pos + 1 >= *n) {
      size_t new_size = *n * 2;
      char *new_ptr = realloc(*lineptr, new_size);
      if (new_ptr == NULL) return -1;
      *lineptr = new_ptr;
      *n = new_size;
    }
    (*lineptr)[pos++] = c;
    if (c == '\n') break;
  }
  if (pos == 0) return -1;
  (*lineptr)[pos] = '\0';
  return pos;
}

static ssize_t getdelim(char **lineptr, size_t *n, int delim, FILE *stream) {
  size_t pos = 0;
  int c;
  if (lineptr == NULL || n == NULL || stream == NULL) {
    errno = EINVAL;
    return -1;
  }
  if (*lineptr == NULL) {
    *n = 128;
    *lineptr = malloc(*n);
    if (*lineptr == NULL) return -1;
  }
  while ((c = fgetc(stream)) != EOF) {
    if (pos + 1 >= *n) {
      size_t new_size = *n * 2;
      char *new_ptr = realloc(*lineptr, new_size);
      if (new_ptr == NULL) return -1;
      *lineptr = new_ptr;
      *n = new_size;
    }
    (*lineptr)[pos++] = c;
    if (c == delim) break;
  }
  if (pos == 0) return -1;
  (*lineptr)[pos] = '\0';
  return pos;
}
#endif

// Function to check if a directory is a Git repository
int is_git_repo(const char *dirpath) {
  DIR *dir;
  struct dirent *entry;
  char git_path[1024];

  snprintf(git_path, sizeof(git_path), "%s/.git", dirpath);
  dir = opendir(git_path);
  if (dir) {
    closedir(dir);
    return 1; // It's a Git repo
  }
  return 0; // Not a Git repo
}

// Function to run a shell command and return its output
char *run_command(const char *cmd) {
  FILE *fp;
  char *result = NULL;
  size_t len = 0;

  fp = popen(cmd, "r");
  if (fp == NULL) {
    perror("Failed to run command");
    return NULL;
  }

  getdelim(&result, &len, '\0', fp);
  pclose(fp);
  return result;
}

// Function to trim trailing newline characters
void trim_newline(char *str) {
  if (str == NULL)
    return;
  size_t len = strlen(str);
  if (len > 0 && str[len - 1] == '\n') {
    str[len - 1] = '\0';
  }
}

// Function to print commit history in JSON-like format
void print_commit_history(const char *dirpath, int num_commits) {
  char commit_cmd[1024];
  snprintf(commit_cmd, sizeof(commit_cmd),
           "cd %s && git log -%d "
           "--pretty=format:'{\"hash\":\"%%H\",\"author\":\"%%an\",\"date\":\"%"
           "%ad\",\"message\":\"%%s\"},' --date=short",
           dirpath, num_commits);

  FILE *fp = popen(commit_cmd, "r");
  if (fp == NULL) {
    perror("Failed to run command");
    return;
  }

  printf("  \"commit_history\": [\n");
  char *line = NULL;
  size_t len = 0;
  int first = 1;

  while (getline(&line, &len, fp) != -1) {
    trim_newline(line);
    if (!first) {
      printf(",\n");
    }
    printf("    %s", line);
    first = 0;
  }

  printf("\n  ],\n");
  free(line);
  pclose(fp);
}

// Function to print available branches in JSON-like format
void print_branches(const char *dirpath) {
  char branches_cmd[1024];
  snprintf(branches_cmd, sizeof(branches_cmd),
           "cd %s && git for-each-ref "
           "--format='{\"name\":\"%%(refname:short)\",\"is_current\":%%(if:"
           "equals=[%%(refname:short)],%%(HEAD))\"true\"%%(then)\"true\"%%("
           "else)\"false\"%%(end)}', refs/heads/",
           dirpath);

  FILE *fp = popen(branches_cmd, "r");
  if (fp == NULL) {
    perror("Failed to run command");
    return;
  }

  printf("  \"branches\": [\n");
  char *line = NULL;
  size_t len = 0;
  int first = 1;

  while (getline(&line, &len, fp) != -1) {
    trim_newline(line);
    if (!first) {
      printf(",\n");
    }
    printf("    %s", line);
    first = 0;
  }

  printf("\n  ]\n");
  free(line);
  pclose(fp);
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Usage: %s <directory_path>\n", argv[0]);
    return 1;
  }

  const char *dirpath = argv[1];

  if (!is_git_repo(dirpath)) {
    printf("{\"error\": \"%s is not a Git repository.\"}\n", dirpath);
    return 1;
  }

  // JSON-like output
  printf("{\n");

  // Repository path
  printf("  \"repository_path\": \"%s\",\n", dirpath);

  // Current branch
  char branch_cmd[1024];
  snprintf(branch_cmd, sizeof(branch_cmd),
           "cd %s && git rev-parse --abbrev-ref HEAD", dirpath);
  char *branch = run_command(branch_cmd);
  trim_newline(branch);
  printf("  \"current_branch\": \"%s\",\n", branch);
  free(branch);

  // Commit history (last 5 commits)
  print_commit_history(dirpath, 5);

  // Available branches
  print_branches(dirpath);

  printf("}\n");

  return 0;
}
