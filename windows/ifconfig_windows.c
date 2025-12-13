// ifconfig_windows.c
#include <iphlpapi.h>
#include <stdio.h>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")

int main() {
  ULONG flags = GAA_FLAG_INCLUDE_PREFIX;
  ULONG outBufLen = 15000; // Initial guess
  PIP_ADAPTER_ADDRESSES pAddresses = NULL;
  PIP_ADAPTER_ADDRESSES pCurrAddresses = NULL;
  PIP_ADAPTER_UNICAST_ADDRESS pUnicast = NULL;
  DWORD dwRetVal = 0;

  // Allocate buffer
  pAddresses = (IP_ADAPTER_ADDRESSES *)malloc(outBufLen);
  if (pAddresses == NULL) {
    printf("Memory allocation failed\n");
    return 1;
  }

  // Call GetAdaptersAddresses
  dwRetVal =
      GetAdaptersAddresses(AF_UNSPEC, flags, NULL, pAddresses, &outBufLen);
  if (dwRetVal == ERROR_BUFFER_OVERFLOW) {
    free(pAddresses);
    pAddresses = (IP_ADAPTER_ADDRESSES *)malloc(outBufLen);
    if (pAddresses == NULL) {
      printf("Memory allocation failed\n");
      return 1;
    }
    dwRetVal =
        GetAdaptersAddresses(AF_UNSPEC, flags, NULL, pAddresses, &outBufLen);
  }

  if (dwRetVal != NO_ERROR) {
    printf("GetAdaptersAddresses failed with error: %lu\n", dwRetVal);
    free(pAddresses);
    return 1;
  }

  // Iterate through adapters
  pCurrAddresses = pAddresses;
  while (pCurrAddresses) {
    printf("\n=== Interface ===\n");
    printf("Name: %wS\n", pCurrAddresses->FriendlyName);
    printf("Description: %wS\n", pCurrAddresses->Description);

    // MAC Address
    if (pCurrAddresses->PhysicalAddressLength != 0) {
      printf("MAC Address: ");
      for (ULONG i = 0; i < pCurrAddresses->PhysicalAddressLength; i++) {
        if (i == pCurrAddresses->PhysicalAddressLength - 1)
          printf("%.2X\n", (int)pCurrAddresses->PhysicalAddress[i]);
        else
          printf("%.2X-", (int)pCurrAddresses->PhysicalAddress[i]);
      }
    }

    // Operational status
    printf("Status: %s\n",
           pCurrAddresses->OperStatus == IfOperStatusUp ? "UP" : "DOWN");

    // IP Addresses
    pUnicast = pCurrAddresses->FirstUnicastAddress;
    while (pUnicast) {
      SOCKET_ADDRESS addr = pUnicast->Address;
      char ipStr[INET6_ADDRSTRLEN];

      if (addr.lpSockaddr->sa_family == AF_INET) {
        inet_ntop(AF_INET, &((struct sockaddr_in *)addr.lpSockaddr)->sin_addr,
                  ipStr, sizeof(ipStr));
        printf("IPv4 Address: %s\n", ipStr);
      } else if (addr.lpSockaddr->sa_family == AF_INET6) {
        inet_ntop(AF_INET6,
                  &((struct sockaddr_in6 *)addr.lpSockaddr)->sin6_addr, ipStr,
                  sizeof(ipStr));
        printf("IPv6 Address: %s\n", ipStr);
      }

      pUnicast = pUnicast->Next;
    }

    pCurrAddresses = pCurrAddresses->Next;
  }

  free(pAddresses);
  return 0;
}
