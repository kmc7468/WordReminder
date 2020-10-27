#include "Window.h"

#include <stdlib.h>
#include <time.h>

static HINSTANCE g_Instance;
static LOGFONT g_GlobalFont;

HFONT GlobalDefaultFont, GlobalBoldFont;

void Initialize(HINSTANCE instance) {
	g_Instance = instance;
	srand((unsigned)time(NULL));

	RegisterWindow(_T("MainWindow"), MainWindowProc);

	g_GlobalFont.lfCharSet = HANGUL_CHARSET;
	_tcscpy(g_GlobalFont.lfFaceName, _T("³ª´®°íµñ"));
	GlobalDefaultFont = CreateGlobalFont(18, false);
	GlobalBoldFont = CreateGlobalFont(18, true);
}
void RegisterWindow(LPCTSTR name, WNDPROC wndProc) {
	WNDCLASS wc;
	wc.cbClsExtra = wc.cbWndExtra = 0;
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hInstance = g_Instance;
	wc.lpfnWndProc = wndProc;
	wc.lpszClassName = name;
	wc.lpszMenuName = NULL;
	wc.style = CS_VREDRAW | CS_HREDRAW;
	RegisterClass(&wc);
}
HWND CreateAndShowWindow(LPCTSTR name, LPCTSTR title, int cmdShow) {
	const HWND handle = CreateWindow(name, title, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 640, 480, NULL, NULL, g_Instance, NULL);
	return ShowWindow(handle, cmdShow), handle;
}
HWND CreateAndShowChild(LPCTSTR name, LPCTSTR text, HFONT font, int flags, int x, int y, int w, int h, HWND parent, int menu) {
	const HWND handle = CreateWindow(name, text, WS_CHILD | WS_VISIBLE | flags, x, y, w, h, parent, (HMENU)menu, g_Instance, NULL);
	if (font) {
		SendMessage(handle, WM_SETFONT, (WPARAM)font, true);
	}
	return handle;
}
HFONT CreateGlobalFont(int height, bool isBold) {
	g_GlobalFont.lfHeight = height;
	g_GlobalFont.lfWeight = isBold ? FW_BOLD : FW_NORMAL;
	return CreateFontIndirect(&g_GlobalFont);
}
void DrawTextUsingFont(HDC dc, HFONT font, int x, int y, LPCTSTR string, int length) {
	const HGDIOBJ oldFont = SelectObject(dc, font);
	TextOut(dc, x, y, string, length);
	SelectObject(dc, oldFont);
}

HWND MainWindow, VocabularyWindow;