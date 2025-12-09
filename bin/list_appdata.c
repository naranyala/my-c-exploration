#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <direct.h> // for _getcwd (not used here, but common)
#include <shlobj.h>
#include <windows.h>
#define GETCWD _getcwd
#else
#include <dirent.h>
#include <pwd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#define GETCWD getcwd
#endif

// Helper: get user's app data / config directory
int get_appdata_dir(char *buffer, size_t buflen) {
#ifdef _WIN32
  if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_APP_DATA, NULL, 0, buffer))) {
    return 1;
  }
  return 0;

#elif __APPLE__
  // macOS: ~/Library/Application Support
  struct passwd *pw = getpwuid(getuid());
  if (!pw)
    return 0;
  snprintf(buffer, buflen, "%s/Library/Application Support", pw->pw_dir);
  return 1;

#else
  // Linux and other Unix-like
  const char *xdg = getenv("XDG_CONFIG_HOME");
  if (xdg && xdg[0] == '/') {
    snprintf(buffer, buflen, "%s", xdg);
  } else {
    struct passwd *pw = getpwuid(getuid());
    if (!pw)
      return 0;
    snprintf(buffer, buflen, "%s/.config", pw->pw_dir);
  }
  return 1;
#endif
}

// Platform-independent function to list directory contents
void list_directory(const char *path) {
#ifdef _WIN32
  char search_path[MAX_PATH];
  snprintf(search_path, sizeof(search_path), "%s\\*", path);

  WIN32_FIND_DATAA findData;
  HANDLE hFind = FindFirstFileA(search_path, &findData);

  if (hFind == INVALID_HANDLE_VALUE) {
    fprintf(stderr, "Cannot open directory: %s\n", path);
    return;
  }

  do {
    if (strcmp(findData.cFileName, ".") == 0 ||
        strcmp(findData.cFileName, "..") == 0)
      continue;

    if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
      printf("[DIR]  %s\n", findData.cFileName);
    } else {
      printf("[FILE] %s\n", findData.cFileName);
    }
  } while (FindNextFileA(hFind, &findData));

  FindClose(hFind);

#else
  DIR *dir = opendir(path);
  if (!dir) {
    perror("Cannot open directory");
    return;
  }

  struct dirent *entry;
  while ((entry = readdir(dir)) != NULL) {
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
      continue;

    // On POSIX, we can't tell dir vs file from dirent alone without stat
    // But for simplicity, we'll assume most are dirs in config folders
    // (or just label all as "ITEM")
    printf("[ITEM] %s\n", entry->d_name);
  }
  closedir(dir);
#endif
}

int main() {
  char appdata_path[4096] = {0};

  if (!get_appdata_dir(appdata_path, sizeof(appdata_path))) {
    fprintf(stderr, "Failed to determine app data directory.\n");
    return EXIT_FAILURE;
  }

  printf("App Data / Config Directory:\n  %s\n\n", appdata_path);
  printf("Contents:\n");
  printf("------------------------\n");

  list_directory(appdata_path);
  return EXIT_SUCCESS;
}
