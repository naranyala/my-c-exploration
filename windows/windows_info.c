#include <lmcons.h> // For UNLEN (username length)
#include <stdio.h>
#include <windows.h>

int main() {
  char username[UNLEN + 1];
  DWORD username_len = UNLEN + 1;
  char sysdir[MAX_PATH];

  // Get current username
  if (GetUserNameA(username, &username_len)) {
    printf("Hello, %s!\n", username);
  } else {
    printf("Failed to get username.\n");
  }

  // Get system directory (e.g., C:\Windows\System32)
  if (GetSystemDirectoryA(sysdir, MAX_PATH)) {
    printf("System Directory: %s\n", sysdir);
  } else {
    printf("Failed to get system directory.\n");
  }

  // Show a Windows message box
  MessageBoxA(NULL, "This is a Windows-native message box from C!",
              "Windows C Program", MB_ICONINFORMATION | MB_OK);

  // Keep console open
  printf("\nPress Enter to exit...");
  getchar();
  return 0;
}
