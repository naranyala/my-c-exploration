#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_FILES 1000
#define MAX_PATH 4096
#define CENTRAL_DIR "/usr/share/applications"

char *files[MAX_FILES];
int file_count = 0;

int is_desktop(const char *name) {
  const char *ext = strrchr(name, '.');
  return ext && strcmp(ext, ".desktop") == 0;
}

void scan(const char *dir) {
  DIR *d = opendir(dir);
  if (!d)
    return;

  struct dirent *ent;
  char path[MAX_PATH];

  while ((ent = readdir(d))) {
    if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
      continue;

    snprintf(path, sizeof(path), "%s/%s", dir, ent->d_name);

    if (ent->d_type == DT_DIR) {
      scan(path);
    } else if (ent->d_type == DT_REG && is_desktop(ent->d_name)) {
      files[file_count++] = strdup(path);
      if (file_count >= MAX_FILES)
        break;
    }
  }
  closedir(d);
}

int main(int argc, char **argv) {
  int move = argc > 1 && strcmp(argv[1], "--move") == 0;
  char *home = getenv("HOME");
  if (!home)
    return 1;

  char dirs[3][MAX_PATH];
  snprintf(dirs[0], MAX_PATH, "%s/Desktop", home);
  snprintf(dirs[1], MAX_PATH, "%s/.local/share/applications", home);
  strcpy(dirs[2], CENTRAL_DIR);

  for (int i = 0; i < 3; i++)
    scan(dirs[i]);

  if (file_count == 0) {
    printf("No .desktop shortcuts found.\n");
    return 2;
  }

  printf("Found %d .desktop file(s):\n", file_count);
  for (int i = 0; i < file_count; i++)
    printf("%s\n", files[i]);
  printf("Total: %d\n\n", file_count);

  if (!move)
    return 0;

  printf("Central folder: %s\n", CENTRAL_DIR);
  printf("Move ALL %d shortcut(s)? [y/N] ", file_count);

  char ans[10];
  fgets(ans, sizeof(ans), stdin);
  ans[strcspn(ans, "\n")] = 0;
  if (ans[0] != 'y' && ans[0] != 'Y') {
    printf("Move cancelled.\n");
    return 3;
  }

  int failed = 0;
  for (int i = 0; i < file_count; i++) {
    char *base = strrchr(files[i], '/') + 1;
    char dst[MAX_PATH];
    snprintf(dst, sizeof(dst), "%s/%s", CENTRAL_DIR, base);

    int counter = 1;
    char temp[MAX_PATH];
    while (access(dst, F_OK) == 0) {
      char *dot = strrchr(base, '.');
      if (dot) {
        snprintf(temp, sizeof(temp), "%.*s.%d%s", (int)(dot - base), base,
                 counter++, dot);
      } else {
        snprintf(temp, sizeof(temp), "%s.%d", base, counter++);
      }
      snprintf(dst, sizeof(dst), "%s/%s", CENTRAL_DIR, temp);
    }

    if (rename(files[i], dst) != 0)
      failed++;
  }

  if (failed > 0) {
    fprintf(stderr, "%d move(s) failed.\n", failed);
    return 4;
  }

  printf("✓ Moved all %d shortcut(s).\n", file_count);

  for (int i = 0; i < file_count; i++)
    free(files[i]);
  return 0;
}
