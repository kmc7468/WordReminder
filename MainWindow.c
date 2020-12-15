#include "Window.h"

#include "Version.h"

static void ShowMultiplayButton();

static HFONT g_TitleFont;
static HWND g_SingleplayButton, g_VocabularyButton, g_LocalMultiplayButton, g_OnlineMultiplayButton;
static HWND g_CreateServerButton, g_JoinServerButton;

static const TCHAR g_Version[] = _T("v") WR_APPLICATION_VERSION;

static Thread g_Thread;
static DWORD WINAPI ShowOnlineMultiplayButtonThread(LPVOID param);

LRESULT CALLBACK MainWindowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam) {
	EVENT {
	case WM_CREATE:
		g_TitleFont = CreateGlobalFont(50, true);

		g_SingleplayButton = CreateAndShowChild(_T("button"), _T("�ܾ� �ϱ��ϱ�"), GlobalBoldFont, BS_PUSHBUTTON,
			WIDTH / 2 - 150, 140, 300, 50, handle, 0);
		g_VocabularyButton = CreateAndShowChild(_T("button"), _T("�ܾ��� �����/�����ϱ�"), GlobalBoldFont, BS_PUSHBUTTON,
			WIDTH / 2 - 150, 200, 300, 50, handle, 1);
		g_LocalMultiplayButton = CreateAndShowChild(_T("button"), _T("���� ��Ƽ �÷���"), GlobalBoldFont, BS_PUSHBUTTON,
			WIDTH / 2 - 150, 260, 300, 50, handle, 2);
		g_OnlineMultiplayButton = CreateAndShowChild(_T("button"), _T("�¶��� ��Ƽ �÷���"), GlobalBoldFont, BS_PUSHBUTTON,
			WIDTH / 2 - 150, 320, 300, 50, handle, 3);

		g_CreateServerButton = CreateAndShowChild(_T("button"), _T("���� �����"), GlobalBoldFont, BS_PUSHBUTTON,
			WIDTH / 2 - 150, 320, 148, 50, handle, 4);
		g_JoinServerButton = CreateAndShowChild(_T("button"), _T("���� �����ϱ�"), GlobalBoldFont, BS_PUSHBUTTON,
			WIDTH / 2 + 2, 320, 148, 50, handle, 5);
		ShowWindow(g_CreateServerButton, SW_HIDE);
		ShowWindow(g_JoinServerButton, SW_HIDE);
		return 0;

	case WM_DESTROY:
		DeleteObject(g_TitleFont);
		StopThread(&g_Thread);

		Destroy();
		return 0;

	case WM_SIZE:
		SetWindowPos(g_SingleplayButton, HWND_TOP, WIDTH / 2 - 150, 140, 0, 0, SWP_NOSIZE);
		SetWindowPos(g_VocabularyButton, HWND_TOP, WIDTH / 2 - 150, 200, 0, 0, SWP_NOSIZE);
		SetWindowPos(g_LocalMultiplayButton, HWND_TOP, WIDTH / 2 - 150, 260, 0, 0, SWP_NOSIZE);
		SetWindowPos(g_OnlineMultiplayButton, HWND_TOP, WIDTH / 2 - 150, 320, 0, 0, SWP_NOSIZE);

		SetWindowPos(g_CreateServerButton, HWND_TOP, WIDTH / 2 - 150, 320, 0, 0, SWP_NOSIZE);
		SetWindowPos(g_JoinServerButton, HWND_TOP, WIDTH / 2 + 2, 320, 0, 0, SWP_NOSIZE);
		return 0;

	case WM_PAINT: {
		BEGINPAINT;
		SetTextAlign(dc, TA_CENTER);

		DrawTextUsingFont(dc, g_TitleFont, WIDTH / 2, 40, STRING("�ܾ� �ϱ� ���α׷�"));
		DrawTextUsingFont(dc, GlobalDefaultFont, WIDTH / 2, 95, STRING("(C) 2020. kmc7468 All rights reserved."));

		DrawTextUsingFont(dc, GlobalDefaultFont, WIDTH / 2, HEIGHT - 70, g_Version, ARRAYSIZE(g_Version));

		return ENDPAINT;
	}

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case 0:
			CreateAndShowWindow(_T("QuestionOptionWindow"), _T("�ܾ� �ϱ��ϱ�"), SW_SHOW);
			EnableWindow(handle, FALSE);
			break;

		case 1:
			VocabularyWindow = CreateAndShowWindow(_T("VocabularyWindow"), _T("�ܾ��� �����/�����ϱ�"), SW_SHOW);
			EnableWindow(handle, FALSE);
			break;

		case 2: {
			const HWND questionOptionWindow = CreateAndShowWindow(_T("QuestionOptionWindow"), _T("���� ��Ƽ �÷���"), SW_SHOW);
			SendMessage(questionOptionWindow, WM_USER + 2, 0, 0);
			EnableWindow(handle, FALSE);
			break;
		}

		case 3:
			ShowWindow(g_OnlineMultiplayButton, SW_HIDE);
			ShowWindow(g_CreateServerButton, SW_SHOW);
			ShowWindow(g_JoinServerButton, SW_SHOW);
			StartThread(&g_Thread, ShowOnlineMultiplayButtonThread, NULL);
			return 0;

		case 4:
		case 5: {
			const WORD id = LOWORD(wParam);
			OnlineMultiplayWindow = CreateAndShowWindow(_T("OnlineMultiplayWindow"), id == 4 ? _T("���� �����") : _T("���� �����ϱ�"), SW_SHOW);
			if (id == 4) {
				SendMessage(OnlineMultiplayWindow, WM_USER + 2, 0, 0);
			}
			EnableWindow(handle, FALSE);
			break;
		}
		}

		StopThread(&g_Thread);
		ShowMultiplayButton();
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

void ShowMultiplayButton() {
	ShowWindow(g_OnlineMultiplayButton, SW_SHOW);
	ShowWindow(g_CreateServerButton, SW_HIDE);
	ShowWindow(g_JoinServerButton, SW_HIDE);
}