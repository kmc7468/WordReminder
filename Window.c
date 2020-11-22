#include "Window.h"

#include "Version.h"

#include <ShlObj.h>
#include <stdlib.h>
#include <time.h>

static HINSTANCE g_Instance;
static LOGFONT g_GlobalFont;

static OPENFILENAME g_FileDialog;
static TCHAR g_FileDialogPath[MAX_PATH];

HFONT GlobalDefaultFont, GlobalBoldFont;

bool Initialize(HINSTANCE instance) {
	g_Instance = instance;
	srand((unsigned)time(NULL));

	LoadSetting();

	RegisterWindow(_T("MainWindow"), MainWindowProc);
	RegisterWindow(_T("VocabularyWindow"), VocabularyWindowProc);
	RegisterWindow(_T("QuestionOptionWindow"), QuestionOptionWindowProc);
	RegisterWindow(_T("QuestionWindow"), QuestionWindowProc);
	RegisterWindow(_T("StatisticWindow"), StatisticWindowProc);
	RegisterWindow(_T("OnlineMultiplayWindow"), OnlineMultiplayWindowProc);
	RegisterWindow(_T("ExaminerWindow"), ExaminerWindowProc);

	g_GlobalFont.lfCharSet = HANGUL_CHARSET;
	_tcscpy(g_GlobalFont.lfFaceName, _T("나눔고딕"));
	GlobalDefaultFont = CreateGlobalFont(18, false);
	GlobalBoldFont = CreateGlobalFont(18, true);

	g_FileDialog.lpstrDefExt = _T("kv");
	g_FileDialog.lpstrFile = g_FileDialogPath;
	g_FileDialog.lpstrFilter = _T("단어장 파일(*.kv, *.kwl)\0*.kv;*.kwl\0모든 파일(*.*)\0*.*\0");
	g_FileDialog.lStructSize = sizeof(g_FileDialog);
	g_FileDialog.nMaxFile = ARRAYSIZE(g_FileDialogPath);

	TCHAR desktop[MAX_PATH];
	SHGetSpecialFolderPath(HWND_DESKTOP, desktop, CSIDL_DESKTOP, FALSE);
	g_FileDialog.lpstrInitialDir = desktop;

	WSADATA data;
	return WSAStartup(MAKEWORD(2, 2), &data) == ERROR_SUCCESS;
}
void Destroy() {
	SaveSetting();
	free(Setting.ServerIp);

	WSACleanup();
}
void RegisterWindow(LPCTSTR name, WNDPROC wndProc) {
	WNDCLASS wc = { 0 };
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

LPCTSTR ShowOpenFileDialog(HWND handle) {
	g_FileDialog.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST;
	g_FileDialog.hwndOwner = handle;
	if (GetOpenFileName(&g_FileDialog)) return g_FileDialogPath;
	return NULL;
}
LPCTSTR ShowSaveFileDialog(HWND handle) {
	g_FileDialog.Flags = OFN_EXPLORER | OFN_HIDEREADONLY;
	g_FileDialog.hwndOwner = handle;
	if (GetSaveFileName(&g_FileDialog)) return g_FileDialogPath;
	return NULL;
}

HWND MainWindow, VocabularyWindow, OnlineMultiplayWindow;

void StartThread(Thread* thread, LPTHREAD_START_ROUTINE function, LPVOID param) {
	thread->Handle = CreateThread(NULL, 0, function, param, 0, &thread->Id);
}
void StopThread(Thread* thread) {
	TerminateThread(thread->Handle, 0);
}