#include <tchar.h>
#include <windows.h>

#define ID_INCREMENT 1001
#define ID_RESET 1002
#define ID_STATIC 1003

static int g_count = 0;
static HWND g_hwndStatic = NULL;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam,
                            LPARAM lParam) {
  switch (uMsg) {
  case WM_CREATE: {
    g_hwndStatic = CreateWindowEx(0, TEXT("STATIC"), TEXT("Count: 0"),
                                  WS_CHILD | WS_VISIBLE | SS_LEFT, 20, 20, 150,
                                  30, hwnd, (HMENU)ID_STATIC, NULL, NULL);

    CreateWindowEx(0, TEXT("BUTTON"), TEXT("Increment"),
                   WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 20, 60, 100, 30, hwnd,
                   (HMENU)ID_INCREMENT, NULL, NULL);

    CreateWindowEx(0, TEXT("BUTTON"), TEXT("Reset"),
                   WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 130, 60, 100, 30,
                   hwnd, (HMENU)ID_RESET, NULL, NULL);
    break;
  }

  case WM_COMMAND: {
    int id = LOWORD(wParam);
    if (id == ID_INCREMENT) {
      g_count++;
      TCHAR buffer[64];
      wsprintf(buffer, TEXT("Count: %d"), g_count); // Safe for small buffers
      SetWindowText(g_hwndStatic, buffer);
    } else if (id == ID_RESET) {
      g_count = 0;
      SetWindowText(g_hwndStatic, TEXT("Count: 0"));
    }
    break;
  }

  case WM_DESTROY:
    PostQuitMessage(0);
    return 0;

  default:
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
  }
  return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow) {
  TCHAR szClassName[] = TEXT("CounterApp");

  WNDCLASS wc = {0};
  wc.lpfnWndProc = WindowProc;
  wc.hInstance = hInstance;
  wc.lpszClassName = szClassName;
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

  if (!RegisterClass(&wc)) {
    MessageBox(NULL, TEXT("Window Registration Failed!"), TEXT("Error"),
               MB_ICONERROR);
    return 1;
  }

  HWND hwnd = CreateWindowEx(
      0, szClassName, TEXT("Counter Demo - Win32 API"),
      WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX, CW_USEDEFAULT,
      CW_USEDEFAULT, // ✅ Fixed typo: CW_USEDEFAULT (no underscore!)
      260, 150, NULL, NULL, hInstance, NULL);

  if (!hwnd) {
    MessageBox(NULL, TEXT("Window Creation Failed!"), TEXT("Error"),
               MB_ICONERROR);
    return 1;
  }

  ShowWindow(hwnd, nCmdShow);
  UpdateWindow(hwnd);

  MSG msg;
  while (GetMessage(&msg, NULL, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  return (int)msg.wParam;
}
