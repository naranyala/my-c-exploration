/*
 *  disk_scan.c – list every fixed drive and its usage
 *  cl /O2 disk_scan.c /link kernel32.lib
 *  gcc -O2 disk_scan.c -lkernel32
 */
// #define _WIN32_WINNT 0x0600 /* Windows Vista+ */
#include <stdio.h>
#include <windows.h>

static const char *unit[] = {" B", " KiB", " MiB", " GiB", " TiB", " PiB"};
static char *fmt_bytes(unsigned long long bytes, char out[16]) {
  int i = 0;
  double val = (double)bytes;
  while (val >= 1024.0 && i < 5) {
    val /= 1024.0;
    ++i;
  }
  sprintf(out, "%.2f%s", val, unit[i]);
  return out;
}

int main(void) {
  char buf[16], drive[] = "A:\\";
  DWORD mask = GetLogicalDrives();
  puts("Windows Storage Scanner\n-----------------------");
  for (int i = 0; i < 26; ++i, mask >>= 1) {
    if (!(mask & 1))
      continue;
    drive[0] = 'A' + i;
    if (GetDriveTypeA(drive) != DRIVE_FIXED)
      continue;

    ULARGE_INTEGER free, total, taken;
    if (!GetDiskFreeSpaceExA(drive, &free, &total, &taken))
      continue;

    char t[16], f[16], u[16];
    printf("%-3s  total=%-10s  free=%-10s  used=%-10s\n", drive,
           fmt_bytes(total.QuadPart, t), fmt_bytes(free.QuadPart, f),
           fmt_bytes(total.QuadPart - free.QuadPart, u));
  }
  return 0;
}
