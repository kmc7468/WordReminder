#include "Window.h"

#include "Http.h"
#include "Version.h"

static void ShowMultiplayButton();

static HFONT g_TitleFont, g_UpdateFont;
static HWND g_SingleplayButton, g_VocabularyButton, g_LocalMultiplayButton, g_OnlineMultiplayButton;
static HWND g_CreateServerButton, g_JoinServerButton;
static HWND g_UpdateButton;

static const TCHAR g_Version[] = _T("v") WR_APPLICATION_VERSION;

static Thread g_ButtonThread, g_UpdateThread;
static DWORD WINAPI ShowOnlineMultiplayButtonThread(LPVOID param);
static DWORD WINAPI UpdateThread(LPVOID param);

LRESULT CALLBACK MainWindowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam) {
	EVENT {
	case WM_CREATE:
		g_TitleFont = CreateGlobalFont(50, true);
		g_UpdateFont = CreateGlobalFont(14, true);

		g_SingleplayButton = CreateAndShowChild(_T("button"), _T("단어 암기하기"), GlobalBoldFont, BS_PUSHBUTTON,
			WIDTH / 2 - 150, 140, 300, 50, handle, 0);
		g_VocabularyButton = CreateAndShowChild(_T("button"), _T("단어장 만들기/수정하기"), GlobalBoldFont, BS_PUSHBUTTON,
			WIDTH / 2 - 150, 200, 300, 50, handle, 1);
		g_LocalMultiplayButton = CreateAndShowChild(_T("button"), _T("로컬 멀티 플레이"), GlobalBoldFont, BS_PUSHBUTTON,
			WIDTH / 2 - 150, 260, 300, 50, handle, 2);
		g_OnlineMultiplayButton = CreateAndShowChild(_T("button"), _T("온라인 멀티 플레이"), GlobalBoldFont, BS_PUSHBUTTON,
			WIDTH / 2 - 150, 320, 300, 50, handle, 3);

		g_CreateServerButton = CreateAndShowChild(_T("button"), _T("서버 만들기"), GlobalBoldFont, BS_PUSHBUTTON,
			WIDTH / 2 - 150, 320, 148, 50, handle, 4);
		g_JoinServerButton = CreateAndShowChild(_T("button"), _T("서버 접속하기"), GlobalBoldFont, BS_PUSHBUTTON,
			WIDTH / 2 + 2, 320, 148, 50, handle, 5);
		ShowWindow(g_CreateServerButton, SW_HIDE);
		ShowWindow(g_JoinServerButton, SW_HIDE);

		StartThread(&g_UpdateThread, UpdateThread, handle);
		return 0;

	case WM_DESTROY:
		DeleteObject(g_TitleFont);
		DeleteObject(g_UpdateFont);
		StopThread(&g_ButtonThread);
		StopThread(&g_UpdateThread);

		Destroy();
		return 0;

	case WM_SIZE:
		SetWindowPos(g_SingleplayButton, HWND_TOP, WIDTH / 2 - 150, 140, 0, 0, SWP_NOSIZE);
		SetWindowPos(g_VocabularyButton, HWND_TOP, WIDTH / 2 - 150, 200, 0, 0, SWP_NOSIZE);
		SetWindowPos(g_LocalMultiplayButton, HWND_TOP, WIDTH / 2 - 150, 260, 0, 0, SWP_NOSIZE);
		SetWindowPos(g_OnlineMultiplayButton, HWND_TOP, WIDTH / 2 - 150, 320, 0, 0, SWP_NOSIZE);

		SetWindowPos(g_CreateServerButton, HWND_TOP, WIDTH / 2 - 150, 320, 0, 0, SWP_NOSIZE);
		SetWindowPos(g_JoinServerButton, HWND_TOP, WIDTH / 2 + 2, 320, 0, 0, SWP_NOSIZE);

		SetWindowPos(g_UpdateButton, HWND_TOP, WIDTH / 2 - 75, HEIGHT - 73, 0, 0, SWP_NOSIZE);
		return 0;

	case WM_PAINT: {
		BEGINPAINT;
		SetTextAlign(dc, TA_CENTER);

		DrawTextUsingFont(dc, g_TitleFont, WIDTH / 2, 40, STRING("단어 암기 프로그램"));
		DrawTextUsingFont(dc, GlobalDefaultFont, WIDTH / 2, 95, STRING("(C) 2020. kmc7468 All rights reserved."));

		DrawTextUsingFont(dc, GlobalDefaultFont, WIDTH / 2, HEIGHT - 70, g_Version, ARRAYSIZE(g_Version));

		return ENDPAINT;
	}

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case 0:
			CreateAndShowWindow(_T("QuestionOptionWindow"), _T("단어 암기하기"), SW_SHOW);
			EnableWindow(handle, FALSE);
			break;

		case 1:
			VocabularyWindow = CreateAndShowWindow(_T("VocabularyWindow"), _T("단어장 만들기/수정하기"), SW_SHOW);
			EnableWindow(handle, FALSE);
			break;

		case 2: {
			const HWND questionOptionWindow = CreateAndShowWindow(_T("QuestionOptionWindow"), _T("로컬 멀티 플레이"), SW_SHOW);
			SendMessage(questionOptionWindow, WM_USER + 2, 0, 0);
			EnableWindow(handle, FALSE);
			break;
		}

		case 3:
			ShowWindow(g_OnlineMultiplayButton, SW_HIDE);
			ShowWindow(g_CreateServerButton, SW_SHOW);
			ShowWindow(g_JoinServerButton, SW_SHOW);
			StartThread(&g_ButtonThread, ShowOnlineMultiplayButtonThread, NULL);
			return 0;

		case 4:
		case 5: {
			const WORD id = LOWORD(wParam);
			OnlineMultiplayWindow = CreateAndShowWindow(_T("OnlineMultiplayWindow"), id == 4 ? _T("서버 만들기") : _T("서버 접속하기"), SW_SHOW);
			if (id == 4) {
				SendMessage(OnlineMultiplayWindow, WM_USER + 2, 0, 0);
			}
			EnableWindow(handle, FALSE);
			break;
		}

		case 6:
			ShellExecute(NULL, _T("open"), _T("https://github.com/kmc7468/WordReminder/releases/latest"), NULL, NULL, SW_SHOWNORMAL);
			break;
		}

		StopThread(&g_ButtonThread);
		ShowMultiplayButton();
		return 0;

	case WM_USER:
		g_UpdateButton = CreateAndShowChild(_T("button"), _T("새 버전이 있습니다"), g_UpdateFont, BS_PUSHBUTTON,
			WIDTH / 2 - 75, HEIGHT - 73, 150, 25, handle, 6);
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

DWORD WINAPI ShowOnlineMultiplayButtonThread(LPVOID param) {
	(void)param;
	Sleep(5000);
	ShowMultiplayButton();
	return 0;
}
DWORD WINAPI UpdateThread(LPVOID param) {
	(void)param;

	HttpRequest request;
	if (!CreateHttpRequest(&request, _T("https://github.com/kmc7468/WordReminder/releases/latest"), _T("GET"), true)) return 0;

	if (SendHttpRequest(&request, NULL)) {
		const LPTSTR result = GetHttpResponseHeader(&request, HeaderLocation);
		const LPTSTR end = result + _tcslen(result);
		if (result) {
			LPTSTR begin = result;
			LPTSTR temp = _tcschr(result, '/');
			while (begin < end && (temp = _tcschr(begin, '/'))) {
				begin = temp + 1;
			}

			if (_tcsncmp(begin, WR_APPLICATION_VERSION, ARRAYSIZE(WR_APPLICATION_VERSION))) {
				SendMessage((HWND)param, WM_USER, 0, 0);
			}
			free(result);
		}
	}

	DestroyHttpRequest(&request);
	return 0;
}

void ShowMultiplayButton() {
	ShowWindow(g_OnlineMultiplayButton, SW_SHOW);
	ShowWindow(g_CreateServerButton, SW_HIDE);
	ShowWindow(g_JoinServerButton, SW_HIDE);
}