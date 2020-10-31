#include "Window.h"

static HFONT g_TitleFont;
static HWND g_StartButton, g_MultiPlayButton, g_EditButton;
static HWND g_CreateServerButton, g_JoinServerButton;

static DWORD g_ThreadId;
static HANDLE g_Thread;

static WINAPI Thread(PVOID param);

LRESULT CALLBACK MainWindowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam) {
	EVENT {
	case WM_CREATE:
		g_TitleFont = CreateGlobalFont(50, true);

		g_StartButton = CreateAndShowChild(_T("button"), _T("단어 암기하기"), GlobalBoldFont, BS_PUSHBUTTON,
			WIDTH / 2 - 150, 140, 300, 50, handle, 0);
		g_MultiPlayButton = CreateAndShowChild(_T("button"), _T("멀티 플레이"), GlobalBoldFont, BS_PUSHBUTTON,
			WIDTH / 2 - 150, 200, 300, 50, handle, 1);
		g_EditButton = CreateAndShowChild(_T("button"), _T("단어장 만들기/수정하기"), GlobalBoldFont, BS_PUSHBUTTON,
			WIDTH / 2 - 150, 260, 300, 50, handle, 2);

		g_CreateServerButton = CreateAndShowChild(_T("button"), _T("서버 만들기"), GlobalBoldFont, BS_PUSHBUTTON,
			WIDTH / 2 - 150, 200, 148, 50, handle, 3);
		g_JoinServerButton = CreateAndShowChild(_T("button"), _T("서버 접속하기"), GlobalBoldFont, BS_PUSHBUTTON,
			WIDTH / 2 + 2, 200, 148, 50, handle, 4);
		ShowWindow(g_CreateServerButton, SW_HIDE);
		ShowWindow(g_JoinServerButton, SW_HIDE);
		return 0;

	case WM_DESTROY:
		DeleteObject(g_TitleFont);
		return 0;

	case WM_SIZE:
		SetWindowPos(g_StartButton, HWND_TOP, WIDTH / 2 - 150, 140, 0, 0, SWP_NOSIZE);
		SetWindowPos(g_MultiPlayButton, HWND_TOP, WIDTH / 2 - 150, 200, 0, 0, SWP_NOSIZE);
		SetWindowPos(g_EditButton, HWND_TOP, WIDTH / 2 - 150, 260, 0, 0, SWP_NOSIZE);

		SetWindowPos(g_CreateServerButton, HWND_TOP, WIDTH / 2 - 150, 200, 0, 0, SWP_NOSIZE);
		SetWindowPos(g_JoinServerButton, HWND_TOP, WIDTH / 2 + 2, 200, 0, 0, SWP_NOSIZE);
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

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case 0:
			CreateAndShowWindow(_T("QuestionOptionWindow"), _T("단어 암기하기"), SW_SHOW);
			EnableWindow(handle, FALSE);
			break;

		case 1:
			ShowWindow(g_MultiPlayButton, SW_HIDE);
			ShowWindow(g_CreateServerButton, SW_SHOW);
			ShowWindow(g_JoinServerButton, SW_SHOW);
			g_Thread = CreateThread(NULL, 0, Thread, NULL, 0, &g_ThreadId);
			break;

		case 2:
			VocabularyWindow = CreateAndShowWindow(_T("VocabularyWindow"), _T("단어장 만들기/수정하기"), SW_SHOW);
			EnableWindow(handle, FALSE);
			break;
		}
		return 0;

	case WM_GETMINMAXINFO: {
		LPMINMAXINFO size = (LPMINMAXINFO)lParam;
		size->ptMinTrackSize.x = 640;
		size->ptMinTrackSize.y = 480;
		return 0;
	}

	case WM_CLOSE:
		DestroyWindow(handle);
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(handle, message, wParam, lParam);
}

WINAPI Thread(PVOID param) {
	Sleep(5000);
	ShowWindow(g_MultiPlayButton, SW_SHOW);
	ShowWindow(g_CreateServerButton, SW_HIDE);
	ShowWindow(g_JoinServerButton, SW_HIDE);
}