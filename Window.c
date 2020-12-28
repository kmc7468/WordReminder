#include "Window.h"

#include "Application.h"

#include <string.h>

void RegisterWindow(LPCTSTR name, WNDPROC wndProc) {
	WNDCLASS wc = { 0 };
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hInstance = Instance;
	wc.lpfnWndProc = wndProc;
	wc.lpszClassName = name;
	wc.lpszMenuName = NULL;
	wc.style = CS_VREDRAW | CS_HREDRAW;

	RegisterClass(&wc);
}
HWND CreateAndShowWindow(LPCTSTR name, LPCTSTR title, int cmdShow) {
	const HWND window = CreateWindow(name, title, WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		CW_USEDEFAULT, CW_USEDEFAULT, 640, 480, NULL, NULL, Instance, NULL);

	ShowWindow(window, cmdShow);
	return window;
}
HWND CreateChild(LPCTSTR name, LPCTSTR text, HFONT font, int flags, int x, int y, int w, int h, HWND parent, int menu) {
	const HWND child = CreateWindow(name, text, WS_CHILD | flags,
		x, y, w, h, parent, (HMENU)(UINT_PTR)menu, Instance, NULL);

	if (font) {
		SendMessage(child, WM_SETFONT, (WPARAM)font, true);
	}
	return child;
}

HWND MainWindow, DialogWindow;

HWND CreateSceneWindow(SUBCLASSPROC windowProc, SUBCLASSPROC sceneProc) {
	const HWND window = CreateWindow(_T("SceneWindow"), NULL, WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		CW_USEDEFAULT, CW_USEDEFAULT, 640, 480, NULL, NULL, Instance, NULL);
	SetWindowSubclass(window, windowProc, 0, 0);
	SendMessage(window, AM_CREATE, 0, 0);

	ChangeScene(window, CreateScene(window, sceneProc));
	return window;
}

HWND CreateScene(HWND window, SUBCLASSPROC sceneProc) {
	RECT clientRect;
	GetClientRect(window, &clientRect);

	const HWND scene = CreateChild(_T("Scene"), NULL, NULL, WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		0, 0, clientRect.right, clientRect.bottom, window, 0);
	SetWindowSubclass(scene, sceneProc, 0, 0);
	SendMessage(scene, AM_CREATE, 0, 0);
	return scene;
}
HWND ChangeScene(HWND window, HWND newScene) {
	const HWND oldScene = (HWND)SendMessage(window, AM_CHANGESCENE, 0, (LPARAM)newScene);
	if (oldScene) {
		ShowWindow(oldScene, SW_HIDE);
	}

	RECT clientRect;
	GetClientRect(window, &clientRect);

	SetWindowPos(newScene, NULL, 0, 0, clientRect.right, clientRect.bottom, SWP_NOZORDER | SWP_NOMOVE);
	return oldScene;
}

int GetAppropriateLengthForDpi(HWND window, int originalLength) {
	return MulDiv(originalLength, GetDpiForWindow(window), USER_DEFAULT_SCREEN_DPI);
}
int GetAppropriateLengthForSize(HWND window, int originalLength) {
	RECT clientRect;
	GetClientRect(window, &clientRect);

	int fitHeight;
	if (clientRect.right * 3 >= clientRect.bottom * 4) {
		fitHeight = clientRect.bottom;
	} else {
		fitHeight = MulDiv(clientRect.right, 3, 4);
	}
	return MulDiv(originalLength, fitHeight, GetAppropriateLengthForDpi(window, 480));
}

HDC StartPaint(HWND window, int width, int height, PaintContext* paintContext) {
	paintContext->OriginalDC = BeginPaint(window, &paintContext->OriginalPaintStruct);

	paintContext->BufferDC = CreateCompatibleDC(paintContext->OriginalDC);
	paintContext->Width = width;
	paintContext->Height = height;

	const HBITMAP bufferBitmap = CreateCompatibleBitmap(paintContext->OriginalDC, width, height);
	paintContext->OriginalBitmap = (HBITMAP)SelectObject(paintContext->BufferDC, bufferBitmap);

	PatBlt(paintContext->BufferDC, 0, 0, width, height, WHITENESS);
	return paintContext->BufferDC;
}
void StopPaint(HWND window, PaintContext* paintContext) {
	BitBlt(paintContext->OriginalDC, 0, 0, paintContext->Width, paintContext->Height, paintContext->BufferDC, 0, 0, SRCCOPY);

	const HBITMAP bufferBitmap = (HBITMAP)SelectObject(paintContext->BufferDC, paintContext->OriginalBitmap);
	DeleteObject(bufferBitmap);
	DeleteDC(paintContext->BufferDC);

	EndPaint(window, &paintContext->OriginalPaintStruct);
}

void DrawString(HDC dc, HFONT font, int x, int y, LPCTSTR string, int length) {
	if (length == -1) {
		length = (int)_tcslen(string);
	}

	const HFONT oldFont = (HFONT)SelectObject(dc, font);
	TextOut(dc, x, y, string, length);
	SelectObject(dc, oldFont);
}