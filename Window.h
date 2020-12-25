#pragma once

#include <stdbool.h>
#include <tchar.h>
#include <WinSock2.h>
#include <Windows.h>

extern HFONT GlobalDefaultFont, GlobalBoldFont;

bool Initialize(HINSTANCE instance);
void Destroy();
void RegisterWindow(LPCTSTR name, WNDPROC wndProc);
HWND CreateAndShowWindow(LPCTSTR name, LPCTSTR title, int cmdShow);
HWND CreateAndShowChild(LPCTSTR name, LPCTSTR text, HFONT font, int flags, int x, int y, int w, int h, HWND parent, int menu);
HFONT CreateGlobalFont(int height, bool isBold);
int GetAppropriateFontSize(int width, int height, int original);

typedef struct {
	HDC OriginalDC;
	HDC BufferDC;
	HBITMAP OriginalBitmap;
	PAINTSTRUCT PaintStruct;
	RECT ClientRect;
} DoubleBufferingContext;

HDC StartDraw(HWND handle, DoubleBufferingContext* context);
void EndDraw(HWND handle, DoubleBufferingContext* context);
void DrawTextUsingFont(HDC dc, HFONT font, int x, int y, LPCTSTR string, int length);

#define BEGINPAINT DoubleBufferingContext context; const HDC dc = StartDraw(handle, &context)
#define ENDPAINT EndDraw(handle, &context), 0

LPCTSTR ShowOpenFileDialog(HWND handle);
LPCTSTR ShowSaveFileDialog(HWND handle);

extern HWND MainWindow, VocabularyWindow, OnlineMultiplayWindow;

LRESULT CALLBACK MainWindowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK VocabularyWindowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK QuestionOptionWindowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK QuestionWindowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK StatisticWindowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK OnlineMultiplayWindowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK ExaminerWindowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK LocalMultiplayWindowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);

#define EVENT RECT windowSize; GetWindowRect(handle, &windowSize); switch (message)
#define WIDTH (windowSize.right - windowSize.left)
#define HEIGHT (windowSize.bottom - windowSize.top)
#define STRING(text) _T(text), sizeof(_T(text)) / sizeof((_T(text))[0]) - 1

typedef struct {
	HANDLE Handle;
	DWORD Id;
} Thread;

void StartThread(Thread* thread, LPTHREAD_START_ROUTINE function, LPVOID param);
void StopThread(Thread* thread);