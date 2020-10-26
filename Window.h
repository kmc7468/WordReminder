#pragma once

#include <stdbool.h>
#include <Windows.h>

extern HFONT GlobalDefaultFont, GlobalBoldFont;

void Initialize(HINSTANCE instance);
void RegisterWindow(LPCTSTR name, WNDPROC wndProc);
HFONT CreateGlobalFont(int height, bool isBold);
void DrawTextUsingFont(HDC dc, HFONT font, int x, int y, LPCTSTR string, int length);