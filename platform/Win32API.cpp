#include "Win32API.hpp"

#include <optional>

bool IsHiDPIAvailable() {
	static std::optional<bool> result;
	if (!result) {
		const HMODULE user32 = GetModuleHandleW(L"User32.dll");
		result =
			GetProcAddress(user32, "SetProcessDpiAwarenessContext") != NULL &&
			GetProcAddress(user32, "GetDpiForWindow") != NULL &&
			GetProcAddress(user32, "AdjustWindowRectExForDpi") != NULL;
	}

	return *result;
}

UINT GetDpiForWindowSafely(HWND window) {
	return IsHiDPIAvailable() ? GetDpiForWindow(window) : USER_DEFAULT_SCREEN_DPI;
}
BOOL AdjustWindowRectExForDpiSafely(LPRECT rect, DWORD style, BOOL hasMenu, DWORD exStyle, UINT dpi) {
	return IsHiDPIAvailable() ? AdjustWindowRectExForDpi(rect, style, hasMenu, exStyle, dpi) : AdjustWindowRectEx(rect, style, hasMenu, exStyle);
}