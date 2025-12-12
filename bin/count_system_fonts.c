// fontcount_unique_parent.c
#define _XOPEN_SOURCE 700
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h> // for strcasecmp
#include <sys/stat.h>

// Windows doesn't have lstat, use stat instead
#ifdef _WIN32
#define lstat stat
#endif

struct NameNode {
  char *name;
  struct NameNode *next;
};

static struct NameNode *set_head = NULL;
static int print_all = 0;

/* check extension */
int is_font_ext(const char *name) {
  const char *ext = strrchr(name, '.');
  if (!ext)
    return 0;
  if (!strcasecmp(ext, ".ttf"))
    return 1;
  if (!strcasecmp(ext, ".otf"))
    return 1;
  if (!strcasecmp(ext, ".ttc"))
    return 1;
  if (!strcasecmp(ext, ".pfb"))
    return 1;
  if (!strcasecmp(ext, ".pfm"))
    return 1;
  if (!strcasecmp(ext, ".woff"))
    return 1;
  if (!strcasecmp(ext, ".woff2"))
    return 1;
  return 0;
}

/* add name to set if missing; return 1 if added, 0 if existed */
int add_unique_name(const char *name) {
  for (struct NameNode *n = set_head; n; n = n->next) {
    if (strcmp(n->name, name) == 0)
      return 0;
  }
  struct NameNode *n = malloc(sizeof(*n));
  if (!n)
    return 0;
  n->name = strdup(name);
  n->next = set_head;
  set_head = n;
  return 1;
}

/* extract last directory component (parent dir) from full path */
char *parent_dir_name(const char *fullpath) {
  char *tmp = strdup(fullpath);
  if (!tmp)
    return NULL;
  char *slash = strrchr(tmp, '/');
  if (!slash) {
    free(tmp);
    return strdup(".");
  }
  *slash = '\0'; /* now tmp ends at parent dir */
  char *last = strrchr(tmp, '/');
  char *result;
  if (!last)
    result = strdup(tmp);
  else
    result = strdup(last + 1);
  free(tmp);
  return result;
}

void traverse(const char *path) {
  DIR *d = opendir(path);
  if (!d)
    return;
  struct dirent *entry;
  while ((entry = readdir(d)) != NULL) {
    if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
      continue;
    size_t len = strlen(path) + strlen(entry->d_name) + 2;
    char *full = malloc(len);
    if (!full) {
      closedir(d);
      return;
    }
    snprintf(full, len, "%s/%s", path, entry->d_name);

    struct stat st;
    if (lstat(full, &st) == 0) {
      if (S_ISDIR(st.st_mode)) {
        traverse(full);
      } else if (S_ISREG(st.st_mode)) {
        if (is_font_ext(full)) {
          char *parent = parent_dir_name(full);
          if (parent) {
            if (add_unique_name(parent) && print_all) {
              printf("new: %s (from %s)\n", parent, full);
            } else if (print_all) {
              printf("seen: %s (from %s)\n", parent, full);
            }
            free(parent);
          }
        }
      }
    }
    free(full);
  }
  closedir(d);
}

void free_set(void) {
  while (set_head) {
    struct NameNode *n = set_head;
    set_head = n->next;
    free(n->name);
    free(n);
  }
}

int main(int argc, char **argv) {
  const char *defaults[] = {"/usr/share/fonts", "/usr/local/share/fonts", NULL};
  int dirs_provided = 0;
  for (int i = 1; i < argc; ++i) {
    if (!strcmp(argv[i], "-a") || !strcmp(argv[i], "--all")) {
      print_all = 1;
      continue;
    }
    if (!strcmp(argv[i], "-d")) {
      if (i + 1 >= argc) {
        fprintf(stderr, "Missing directory after -d\n");
        return 1;
      }
      traverse(argv[++i]);
      dirs_provided = 1;
      continue;
    }
    if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")) {
      printf("Usage: %s [-a] [-d dir]...\n  -a print each discovered parent "
             "name\n",
             argv[0]);
      return 0;
    }
    fprintf(stderr, "Unknown arg: %s\n", argv[i]);
    return 1;
  }

  if (!dirs_provided) {
    for (int i = 0; defaults[i]; ++i)
      traverse(defaults[i]);
  }

  /* count unique parent names */
  int count = 0;
  for (struct NameNode *n = set_head; n; n = n->next)
    count++;
  printf("Unique parent directory names (font families): %d\n", count);
  if (!print_all) {
    for (struct NameNode *n = set_head; n; n = n->next)
      printf("  %s\n", n->name);
  }

  free_set();
  return 0;
}
