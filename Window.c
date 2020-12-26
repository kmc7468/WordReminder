#include "Window.h"

#include "Version.h"

#include <ShlObj.h>
#include <stdlib.h>
#include <time.h>

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
	const HWND handle = CreateWindow(name, title, WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
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

HDC StartDraw(HWND handle, DoubleBufferingContext* context) {
	context->OriginalDC = BeginPaint(handle, &context->PaintStruct);
	context->BufferDC = CreateCompatibleDC(context->OriginalDC);

	GetClientRect(handle, &context->ClientRect);
	const HBITMAP bufferBitmap = CreateCompatibleBitmap(context->OriginalDC, context->ClientRect.right, context->ClientRect.bottom);
	context->OriginalBitmap = SelectObject(context->BufferDC, bufferBitmap);

	PatBlt(context->BufferDC, 0, 0, context->ClientRect.right, context->ClientRect.bottom, WHITENESS);
	return context->BufferDC;
}
void EndDraw(HWND handle, DoubleBufferingContext* context) {
	BitBlt(context->OriginalDC, 0, 0, context->ClientRect.right, context->ClientRect.bottom, context->BufferDC, 0, 0, SRCCOPY);

	const HBITMAP bufferBitmap = SelectObject(context->BufferDC, context->OriginalBitmap);
	DeleteObject(bufferBitmap);
	DeleteDC(context->BufferDC);

	EndPaint(handle, &context->PaintStruct);
}
void DrawTextUsingFont(HDC dc, HFONT font, int x, int y, LPCTSTR string, int length) {
	const HGDIOBJ oldFont = SelectObject(dc, font);
	TextOut(dc, x, y, string, length);
	SelectObject(dc, oldFont);
}

HWND MainWindow, VocabularyWindow, OnlineMultiplayWindow;

void StartThread(Thread* thread, LPTHREAD_START_ROUTINE function, LPVOID param) {
	thread->Handle = CreateThread(NULL, 0, function, param, 0, &thread->Id);
}
void StopThread(Thread* thread) {
	TerminateThread(thread->Handle, 0);
}