#include "WinAPI.h"

#include "Application.h"

UINT GetDpiForWindowSafely(HWND window) {
#if WR_APPLICATION_ENABLE_DPI_AWARENESS
	return GetDpiForWindow(window);
#else
	(void)window;
	return USER_DEFAULT_SCREEN_DPI;
#endif
}
BOOL AdjustWindowRectExForDpiSafely(LPRECT rect, DWORD style, BOOL hasMenu, DWORD exStyle, UINT dpi) {
#if WR_APPLICATION_ENABLE_DPI_AWARENESS
	return AdjustWindowRectExForDpi(rect, style, hasMenu, exStyle, dpi);
#else
	(void)dpi;
	return AdjustWindowRectEx(rect, style, hasMenu, exStyle)(;
#endif
}