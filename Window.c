#include "Window.h"

#include <stdlib.h>
#include <tchar.h>
#include <time.h>

static HINSTANCE g_Instance;
static LOGFONT g_GlobalFont;

HFONT GlobalDefaultFont, GlobalBoldFont;

void Initialize(HINSTANCE instance) {
	g_Instance = instance;
	srand((unsigned)time(NULL));

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