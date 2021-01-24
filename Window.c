#include "Window.h"

#include "Application.h"
#include "UIEngine.h"

#include <math.h>
#include <stdlib.h>
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
HWND CreateChild(LPCTSTR name, LPCTSTR text, HFONT font, int flags, int x, int y, int w, int h, HWND parent, int menu) {
	const HWND child = CreateWindow(name, text, WS_CHILD | flags,
		x, y, w, h, parent, (HMENU)(UINT_PTR)menu, Instance, NULL);

	if (font) {
		SetFont(child, font);
	}
	return child;
}
HWND CreateButton(LPCTSTR text, int flags, HWND parent, int menu) {
	return CreateChild(_T("button"), text, NULL, BS_PUSHBUTTON | flags, 0, 0, 0, 0, parent, menu);
}
HWND CreateCheckBox(LPCTSTR text, int flags, HWND parent, int menu) {
	return CreateChild(_T("button"), text, NULL, BS_AUTOCHECKBOX | flags, 0, 0, 0, 0, parent, menu);
}
HWND CreateRadioButton(LPCTSTR text, int flags, HWND parent, int menu) {
	return CreateChild(_T("button"), text, NULL, BS_AUTORADIOBUTTON | flags, 0, 0, 0, 0, parent, menu);
}
HWND CreateStatic(LPCTSTR text, int flags, HWND parent, int menu) {
	return CreateChild(_T("static"), text, NULL, flags, 0, 0, 0, 0, parent, menu);
}
HWND CreateList(int flags, HWND parent, int menu) {
	return CreateChild(_T("listbox"), NULL, NULL, WS_BORDER | WS_VSCROLL | LBS_NOTIFY | LBS_NOINTEGRALHEIGHT | flags, 0, 0, 0, 0, parent, menu);
}
HWND CreateEdit(int flags, HWND parent, int menu) {
	return CreateChild(_T("edit"), NULL, NULL, WS_BORDER | flags, 0, 0, 0, 0, parent, menu);
}
void SetFont(HWND window, HFONT font) {
	SendMessage(window, WM_SETFONT, (WPARAM)font, true);
}

HWND MainWindow, DialogWindow;

HWND CreateSceneWindow(SUBCLASSPROC windowProc, SUBCLASSPROC sceneProc) {
	RECT rect = { 0, 0, 640, 480 };
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

	const HWND window = CreateWindow(_T("SceneWindow"), _T(""), WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top, NULL, NULL, Instance, NULL);
	SetWindowSubclass(window, windowProc, 0, 0);
	SendMessage(window, AM_CREATE, 0, 0);
	SendMessage(window, WM_SIZE, 0, MAKELPARAM(640, 480));

	ChangeScene(window, CreateScene(window, sceneProc));
	return window;
}

#define PROP_UIENGINE		_T("UIEngine")

HWND CreateScene(HWND window, SUBCLASSPROC sceneProc) {
	RECT clientRect;
	GetClientRect(window, &clientRect);

	const HWND scene = CreateChild(_T("Scene"), NULL, NULL, WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		0, 0, clientRect.right, clientRect.bottom, window, 0);
	SetWindowSubclass(scene, sceneProc, 0, 0);
	SendMessage(scene, AM_CREATE, 0, 0);
	SendMessage(scene, AM_CREATEUI, 0, (LPARAM)GetProp(scene, PROP_UIENGINE));
	SendMessage(scene, WM_SIZE, 0, MAKELPARAM(clientRect.right, clientRect.bottom));
	return scene;
}
HWND GetScene(HWND window) {
	return (HWND)SendMessage(window, AM_CHANGESCENE, 0, 0);
}
HWND ChangeScene(HWND window, HWND newScene) {
	const HWND oldScene = (HWND)SendMessage(window, AM_CHANGESCENE, 0, (LPARAM)newScene);
	SendMessage(oldScene, AM_DEACTIVATE, 0, 0);

	RECT clientRect;
	GetClientRect(window, &clientRect);
	SetWindowPos(newScene, HWND_TOP, 0, 0, clientRect.right, clientRect.bottom, SWP_NOMOVE);
	SendMessage(newScene, AM_ACTIVATE, 0, 0);

	ShowWindow(newScene, SW_SHOW);
	ShowWindow(oldScene, SW_HIDE);
	return oldScene;
}
void SetSceneTitle(HWND scene, LPCTSTR newTitle) {
	static const TCHAR defaultTitle[] = _T("단어 암기 프로그램 ") WR_APPLICATION_VERSION;
	static const TCHAR slash[] = _T(" - ");

	int bufferLength = ARRAYSIZE(defaultTitle);
	if (newTitle) {
		bufferLength += ARRAYSIZE(slash) + (int)_tcslen(newTitle) - 1;
	}

	LPTSTR buffer = malloc(sizeof(TCHAR) * bufferLength);
	_tcscpy(buffer, defaultTitle);
	if (newTitle) {
		_tcscat(buffer, slash);
		_tcscat(buffer, newTitle);
	}

	SetWindowText(GetParent(scene), buffer);
	free(buffer);
}

float GetAppropriateFloatLengthForDpi(HWND window, float originalLength) {
	return originalLength * GetDpiForWindowSafely(window) / USER_DEFAULT_SCREEN_DPI;
}
int GetAppropriateLengthForDpi(HWND window, int originalLength) {
	return (int)floorf(GetAppropriateFloatLengthForDpi(window, (float)originalLength) + 0.5f);
}
float GetAppropriateFloatLengthForSize(HWND window, float originalLength) {
	RECT clientRect;
	GetClientRect(window, &clientRect);

	float fitHeight;
	if (clientRect.right * 3 >= clientRect.bottom * 4) {
		fitHeight = (float)clientRect.bottom;
	} else {
		fitHeight = clientRect.right * 3.f / 4;
	}
	return originalLength * fitHeight / GetAppropriateLengthForDpi(window, 480);
}
int GetAppropriateLengthForSize(HWND window, int originalLength) {
	return (int)floorf(GetAppropriateFloatLengthForSize(window, (float)originalLength) + 0.5f);
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

#define PROP_CURRENT_DPI	_T("CurrentDPI")
#define PROP_CURRENT_SCENE	_T("CurrentScene")

LRESULT CALLBACK SceneWindowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_CREATE:
		SetProp(handle, PROP_CURRENT_DPI, (HANDLE)USER_DEFAULT_SCREEN_DPI);
		return 0;

	case WM_SIZE: {
		const HWND scene = GetProp(handle, PROP_CURRENT_SCENE);
		SetWindowPos(scene, NULL, 0, 0, LOWORD(lParam), HIWORD(lParam), SWP_NOZORDER | SWP_NOMOVE);
		return 0;
	}

	case WM_DPICHANGED: {
		RECT rect;
		GetClientRect(handle, &rect);

		const int oldDpi = (int)(UINT_PTR)GetProp(handle, PROP_CURRENT_DPI);
		const int newDpi = LOWORD(wParam);

		rect.right = MulDiv(rect.right, newDpi, oldDpi);
		rect.bottom = MulDiv(rect.bottom, newDpi, oldDpi);

		const HWND scene = GetProp(handle, PROP_CURRENT_SCENE);
		SetWindowPos(scene, NULL, 0, 0, rect.right, rect.bottom, SWP_NOZORDER | SWP_NOMOVE);

		AdjustWindowRectExForDpiSafely(&rect, WS_OVERLAPPEDWINDOW, FALSE, 0, newDpi);
		SetWindowPos(handle, NULL, 0, 0, rect.right - rect.left, rect.bottom - rect.top, SWP_NOZORDER | SWP_NOMOVE);
		SetProp(handle, PROP_CURRENT_DPI, (HANDLE)(UINT_PTR)newDpi);
		return 0;
	}

	case AM_CHANGESCENE: {
		const HWND oldScene = GetProp(handle, PROP_CURRENT_SCENE);
		if (lParam) {
			ShowWindow(oldScene, SW_HIDE);

			SetProp(handle, PROP_CURRENT_SCENE, (HWND)lParam);
			ShowWindow((HWND)lParam, SW_SHOW);
		}
		return (LRESULT)oldScene;
	}

	case WM_CLOSE:
		DestroyWindow(handle);
		return 0;

	default:
		return DefWindowProc(handle, message, wParam, lParam);
	}
}
LRESULT CALLBACK MainWindowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR dummy0, DWORD_PTR dummy1) {
	EVENT {
	case WM_GETMINMAXINFO: {
		clientRect.right = GetAppropriateLengthForDpi(handle, 640);
		clientRect.bottom = GetAppropriateLengthForDpi(handle, 480);
		AdjustWindowRectExForDpiSafely(&clientRect, WS_OVERLAPPEDWINDOW, FALSE, 0, GetDpiForWindowSafely(handle));

		LPMINMAXINFO size = (LPMINMAXINFO)lParam;
		size->ptMinTrackSize.x = clientRect.right - clientRect.left;
		size->ptMinTrackSize.y = clientRect.bottom - clientRect.top;
		return 0;
	}

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	default:
		return DefSubclassProc(handle, message, wParam, lParam);
	}
}

LRESULT CALLBACK SceneProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_CREATE: {
		UIEngine* const uiEngine = calloc(1, sizeof(UIEngine));
		CreateUIEngine(uiEngine);
		SetProp(handle, PROP_UIENGINE, (HANDLE)uiEngine);
		return 0;
	}

	case WM_DESTROY: {
		UIEngine* const uiEngine = (UIEngine*)GetProp(handle, PROP_UIENGINE);
		DestroyUIEngine(uiEngine);
		free(uiEngine);

		SendMessage(handle, AM_DESTROY, 0, 0);
		return DefWindowProc(handle, message, wParam, lParam);
	}

	case WM_SIZE: {
		UIEngine* const uiEngine = (UIEngine*)GetProp(handle, PROP_UIENGINE);
		EvaluateUIEngine(uiEngine, handle, LOWORD(lParam), HIWORD(lParam));
		UpdateUIEngine(uiEngine);
		return DefWindowProc(handle, message, wParam, lParam);
	}

	case WM_PAINT:
		SendMessage(handle, AM_PAINT, 0, (LPARAM)GetProp(handle, PROP_UIENGINE));
		return DefWindowProc(handle, message, wParam, lParam);

	case WM_CTLCOLORSTATIC: {
		const HDC dc = (HDC)wParam;
		SetBkMode(dc, TRANSPARENT);
		return 0;
	}

	default:
		return DefWindowProc(handle, message, wParam, lParam);
	}
}