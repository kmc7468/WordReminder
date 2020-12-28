#pragma once

#include "WinAPI.h"

#include <stdbool.h>

void RegisterWindow(LPCTSTR className, WNDPROC wndProc);
HWND CreateChild(LPCTSTR className, LPCTSTR text, HFONT font, int flags, int x, int y, int w, int h, HWND parent, int menu);

extern HWND MainWindow, DialogWindow;

HWND CreateSceneWindow(SUBCLASSPROC windowProc, SUBCLASSPROC sceneProc);

HWND CreateScene(HWND window, SUBCLASSPROC sceneProc);
HWND ChangeScene(HWND window, HWND newScene);

int GetAppropriateLengthForDpi(HWND window, int originalLength);
int GetAppropriateLengthForSize(HWND window, int originalLength);

typedef struct {
	HDC OriginalDC;
	HBITMAP OriginalBitmap;
	PAINTSTRUCT OriginalPaintStruct;

	HDC BufferDC;
	int Width, Height;
} PaintContext;

HDC StartPaint(HWND window, int width, int height, PaintContext* paintContext);
void StopPaint(HWND window, PaintContext* paintContext);

void DrawString(HDC dc, HFONT font, int x, int y, LPCTSTR string, int length);

#define STARTPAINT PaintContext paintContext; const HDC dc = StartPaint(handle, WIDTH, HEIGHT, &paintContext)
#define STOPPAINT StopPaint(handle, &paintContext), 0

#define EVENT (void)(dummy0, dummy1); RECT clientRect; GetClientRect(handle, &clientRect); switch (message)
#define WIDTH clientRect.right
#define HEIGHT clientRect.bottom
#define CSTR(string) _T(string), ARRAYSIZE(_T(string))

#define AM_CREATE WM_APP + 0
#define AM_CHANGESCENE WM_USER + 1

LRESULT CALLBACK MainWindowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR dummy0, DWORD_PTR dummy1);