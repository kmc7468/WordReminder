#include "Window.h"

static HFONT g_TitleFont;
static HWND g_StartButton, g_EditButton;

LRESULT CALLBACK MainWindowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam) {
	EVENT {
	case WM_CREATE:
		g_TitleFont = CreateGlobalFont(50, true);

		g_StartButton = CreateAndShowChild(_T("button"), _T("단어 암기하기"), GlobalBoldFont, BS_PUSHBUTTON,
			WIDTH / 2 - 150, 140, 300, 50, handle, 0);
		g_EditButton = CreateAndShowChild(_T("button"), _T("단어장 만들기/수정하기"), GlobalBoldFont, BS_PUSHBUTTON,
			WIDTH / 2 - 150, 200, 300, 50, handle, 1);
		return 0;

	case WM_DESTROY:
		DeleteObject(g_EditButton);
		return 0;

	case WM_SIZE:
		SetWindowPos(g_StartButton, HWND_TOP, WIDTH / 2 - 150, 140, 0, 0, SWP_NOSIZE);
		SetWindowPos(g_EditButton, HWND_TOP, WIDTH / 2 - 150, 200, 0, 0, SWP_NOSIZE);
		return 0;

	case WM_PAINT: {
		PAINTSTRUCT ps;
		const HDC dc = BeginPaint(handle, &ps);
		SetTextAlign(dc, TA_CENTER);

		DrawTextUsingFont(dc, g_TitleFont, WIDTH / 2, 40, STRING("단어 암기 프로그램"));
		DrawTextUsingFont(dc, GlobalDefaultFont, WIDTH / 2, 95, STRING("(C) 2020. kmc7468 All rights reserved."));

		EndPaint(handle, &ps);
		return 0;
	}

	case WM_GETMINMAXINFO: {
		LPMINMAXINFO size = (LPMINMAXINFO)lParam;
		size->ptMinTrackSize.x = 640;
		size->ptMinTrackSize.y = 480;
		return 0;
	}

	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(handle, message, wParam, lParam);
}