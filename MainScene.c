#include "Window.h"

#define AM_HASUPDATE	AM_USER + 0
#define AM_MERGEBUTTON	AM_USER + 1

#define AM_STOPTHREAD	AM_USER + 0

#include "Application.h"
#include "Http.h"
#include "UIEngine.h"

#include <stdlib.h>
#include <string.h>

static HWND g_TitleStatic, g_CopyrightStatic, g_VersionStatic;

static HWND g_SingleplayButton, g_VocabularyButton, g_LocalMultiplayButton, g_OnlineMultiplayButton;
static HWND g_CreateServerButton, g_JoinServerButton;

static HWND g_UpdateButton;
static Thread g_UpdateCheckThread;
static DWORD WINAPI UpdateCheckThread(LPVOID param);

LRESULT CALLBACK MainSceneProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR dummy0, DWORD_PTR dummy1) {
	EVENT {
	case AM_CREATE: {
		g_TitleStatic = CreateStatic(_T("�ܾ� �ϱ� ���α׷�"), WS_VISIBLE | SS_CENTER, handle, -1);
		g_CopyrightStatic = CreateStatic(_T("(C) 2020-2021. kmc7468 All rights reserved."), WS_VISIBLE | SS_CENTER, handle, -1);
		g_VersionStatic = CreateStatic(_T("v") WR_APPLICATION_VERSION, WS_VISIBLE | SS_CENTER, handle, -1);

		g_SingleplayButton = CreateButton(_T("�ܾ� �ϱ��ϱ�"), WS_VISIBLE, handle, 0);
		g_VocabularyButton = CreateButton(_T("�ܾ��� �����ϱ�"), WS_VISIBLE, handle, 1);
		g_LocalMultiplayButton = CreateButton(_T("���� ��Ƽ �÷���"), WS_VISIBLE, handle, 2);
		g_OnlineMultiplayButton = CreateButton(_T("�¶��� ��Ƽ �÷���"), WS_VISIBLE, handle, 3);

		g_CreateServerButton = CreateButton(_T("���� �����"), 0, handle, 4);
		g_JoinServerButton = CreateButton(_T("���� �����ϱ�"), 0, handle, 5);

		g_UpdateButton = CreateButton(_T("�� ������ �ֽ��ϴ�"), 0, handle, 6);
		StartThread(&g_UpdateCheckThread, UpdateCheckThread, handle);
		return 0;
	}

	case AM_CREATEUI: {
		UIEngine* const uiEngine = (UIEngine*)lParam;

		UIFONT_CON(titleFont, 50, true);
		UIFONT_CON(copyrightFont, 18, false);
		UIFONT_CON(buttonFont, 18, true);
		UIFONT_CON(updateButtonFont, 14, true);

		uiEngine->RootComponent.Alignment = Center;

		UICOMP_DOH(area, Horizontal, CenterWithMargin, 85, &uiEngine->RootComponent);

		UICOMP_DOC(titleSection, Horizontal, None, area);
		UICOMP_CON_W(titleBar, &g_TitleStatic, Horizontal, None, 50, titleFont, titleSection);
		UICOMP_CON(margin1, Horizontal, None, 5, titleSection);
		UICOMP_CON_W(copyrightBar, &g_CopyrightStatic, Horizontal, None, 18, copyrightFont, titleSection);

		UICOMP_CON(buttonSection1, Horizontal, Center, 230, area);
		UICOMP_CON(buttonSection2, Vertical, None, 300, buttonSection1);
		UICOMP_CON_W(singleplayButton, &g_SingleplayButton, Horizontal, None, 50, buttonFont, buttonSection2);
		UICOMP_CON(buttonMargin1, Horizontal, None, 10, buttonSection2);
		UICOMP_CON_W(vocabularyButton, &g_VocabularyButton, Horizontal, None, 50, buttonFont, buttonSection2);
		UICOMP_CON(buttonMargin2, Horizontal, None, 10, buttonSection2);
		UICOMP_CON_W(localMultiplayButton, &g_LocalMultiplayButton, Horizontal, None, 50, buttonFont, buttonSection2);
		UICOMP_CON(buttonMargin3, Horizontal, None, 10, buttonSection2);
		UICOMP_CON_W(onlineMultiplayButton, &g_OnlineMultiplayButton, Horizontal, CenterWithMargin, 50, buttonFont, buttonSection2);

		UICOMP_CON_W(createServerButton, &g_CreateServerButton, Vertical, None, 145, buttonFont, onlineMultiplayButton);
		UICOMP_CON_W(joinServerButton, &g_JoinServerButton, Vertical, None, 145, buttonFont, onlineMultiplayButton);

		UICOMP_CON(versionSection, Horizontal, Center, 18, area);
		UICOMP_CON_W(versionBar, &g_VersionStatic, Vertical, None, 150, copyrightFont, versionSection);
		UICOMP_DOH_W(updateButton, &g_UpdateButton, Horizontal, None, 100, updateButtonFont, versionBar);
		return 0;
	}

	case AM_ACTIVATE:
		SetSceneTitle(handle, NULL);
		return 0;

	case AM_HASUPDATE:
		ShowWindow(g_VersionStatic, SW_HIDE);

		ShowWindow(g_UpdateButton, SW_SHOW);
		return 0;

	case WM_DESTROY:
		DestroyThread(&g_UpdateCheckThread);
		return 0;

	case WM_CTLCOLORSTATIC: {
		const HDC dc = (HDC)wParam;
		SetBkMode(dc, TRANSPARENT);
		return 0;
	}

	default:
		return DefSubclassProc(handle, message, wParam, lParam);
	}
}

DWORD WINAPI UpdateCheckThread(LPVOID param) {
	HttpRequest request;
	if (!CreateHttpRequest(&request, _T("https://github.com/kmc7468/WordReminder/releases/latest"), _T("GET"), true)) return 0;

	if (SendHttpRequest(&request, NULL)) {
		const LPTSTR result = GetHttpResponseHeader(&request, Location);
		if (result) {
			const LPTSTR end = result + _tcslen(result);
			LPTSTR begin = result, temp;
			while (begin < end && (temp = _tcschr(begin, '/'))) {
				begin = temp + 1;
			}

			if (_tcsncmp(begin, WR_APPLICATION_VERSION, ARRAYSIZE(WR_APPLICATION_VERSION) - 1)) {
				SendMessage((HWND)param, AM_HASUPDATE, 0, 0);
			}

			free(result);
		}
	}

	DestroyHttpRequest(&request);
	return 0;
}