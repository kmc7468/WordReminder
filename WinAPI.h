#pragma once

#include <WinSock2.h>
#include <Windows.h>
#include <winhttp.h>

#include <CommCtrl.h>
#include <tchar.h>

UINT GetDpiForWindowSafely(HWND window);
BOOL AdjustWindowRectExForDpiSafely(LPRECT rect, DWORD style, BOOL hasMenu, DWORD exStyle, UINT dpi);