#include <lm.h>
#include <stdio.h>
#include <windows.h>

#pragma comment(lib, "netapi32.lib")

int main() {
  LPUSER_INFO_0 pBuf = NULL;
  DWORD dwEntriesRead = 0;
  DWORD dwTotalEntries = 0;
  NET_API_STATUS nStatus;

  printf("Local User Accounts:\n");
  printf("--------------------\n");

  nStatus = NetUserEnum(NULL,                  // local computer
                        0,                     // level 0 (just names)
                        FILTER_NORMAL_ACCOUNT, // normal accounts only
                        (LPBYTE *)&pBuf,       // output buffer
                        MAX_PREFERRED_LENGTH,  // no size limit
                        &dwEntriesRead,        // entries read
                        &dwTotalEntries,       // total entries
                        NULL);                 // no resume handle

  if (nStatus == NERR_Success) {
    for (DWORD i = 0; i < dwEntriesRead; i++) {
      wprintf(L"%s\n", pBuf[i].usri0_name);
    }
    NetApiBufferFree(pBuf);
    printf("\nTotal users: %lu\n", dwEntriesRead);
  } else {
    fprintf(stderr, "Error: %lu\n", nStatus);
    return 1;
  }

  return 0;
}
