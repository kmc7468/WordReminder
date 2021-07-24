#pragma once

#include <WinSock2.h>
#include <Windows.h>
#include <winhttp.h>

bool IsHiDPIAvailable();

UINT GetDpiForWindowSafely(HWND window);
BOOL AdjustWindowRectExForDpiSafely(LPRECT rect, DWORD style, BOOL hasMenu, DWORD exStyle, UINT dpi);