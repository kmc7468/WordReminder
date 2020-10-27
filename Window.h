#pragma once

#include <stdbool.h>
#include <tchar.h>
#include <Windows.h>

extern HFONT GlobalDefaultFont, GlobalBoldFont;

void Initialize(HINSTANCE instance);
void RegisterWindow(LPCTSTR name, WNDPROC wndProc);
HWND CreateAndShowWindow(LPCTSTR name, LPCTSTR title, int cmdShow);
HWND CreateAndShowChild(LPCTSTR name, LPCTSTR text, HFONT font, int flags, int x, int y, int w, int h, HWND parent, int menu);
HFONT CreateGlobalFont(int height, bool isBold);
void DrawTextUsingFont(HDC dc, HFONT font, int x, int y, LPCTSTR string, int length);

extern HWND MainWindow, VocabularyWindow;

LRESULT CALLBACK MainWindowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK VocabularyWindowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK QuestionOptionWindowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK QuestionWindowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);

#define EVENT RECT windowSize; GetWindowRect(handle, &windowSize); switch (message)
#define WIDTH (windowSize.right - windowSize.left)
#define HEIGHT (windowSize.bottom - windowSize.top)
#define STRING(text) _T(text), sizeof(_T(text)) / sizeof((_T(text))[0])