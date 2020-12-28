#include "Window.h"

LRESULT CALLBACK MainSceneProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR dummy0, DWORD_PTR dummy1) {
	EVENT {
	default:
		return DefSubclassProc(handle, message, wParam, lParam);
	}
}