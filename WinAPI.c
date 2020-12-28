#include "WinAPI.h"

#include "Application.h"

UINT GetDpiForWindowSafely(HWND window) {
#if WR_APPLICATION_ENABLE_DPI_AWARENESS
	return GetDpiForWindow(window);
#else
	return USER_DEFAULT_SCREEN_DPI;
#endif
}