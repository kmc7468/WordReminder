#include "WinAPI.h"

#include "Application.h"

static int g_IsHiDPIAvailable = -1;

bool IsHiDPIAvailable() {
	if (g_IsHiDPIAvailable == -1) {
		g_IsHiDPIAvailable =
			GetProcAddress(GetModuleHandle(_T("User32.dll")), "SetProcessDpiAwarenessContext") != NULL &&
			GetProcAddress(GetModuleHandle(_T("User32.dll")), "GetDpiForWindow") != NULL &&
			GetProcAddress(GetModuleHandle(_T("User32.dll")), "AdjustWindowRectExForDpi") != NULL;
	}
	return g_IsHiDPIAvailable;
}

UINT GetDpiForWindowSafely(HWND window) {
	const UINT windowDpi = IsHiDPIAvailable() ? GetDpiForWindow(window) : USER_DEFAULT_SCREEN_DPI;
	return MulDiv(windowDpi, Setting.Scale, 100);
}
BOOL AdjustWindowRectExForDpiSafely(LPRECT rect, DWORD style, BOOL hasMenu, DWORD exStyle, UINT dpi) {
	if (IsHiDPIAvailable()) return AdjustWindowRectExForDpi(rect, style, hasMenu, exStyle, dpi);
	else return AdjustWindowRectEx(rect, style, hasMenu, exStyle);
}