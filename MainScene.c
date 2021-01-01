#include "Window.h"

#define AM_HASUPDATE	AM_USER + 0

#include "Application.h"
#include "Http.h"
#include "UIEngine.h"

#include <stdlib.h>
#include <string.h>

static HWND g_SingleplayButton, g_VocabularyButton, g_LocalMultiplayButton, g_OnlineMultiplayButton;
static HWND g_CreateServerButton, g_JoinServerButton;

static const TCHAR g_Version[] = _T("v") WR_APPLICATION_VERSION;

static HWND g_UpdateButton;
static Thread g_UpdateCheckThread;
static DWORD WINAPI UpdateCheckThread(LPVOID param);

LRESULT CALLBACK MainSceneProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR dummy0, DWORD_PTR dummy1) {
	EVENT {
	case AM_CREATE: {
		g_SingleplayButton = CreateChild(_T("button"), _T("단어 암기하기"), NULL, WS_VISIBLE | BS_PUSHBUTTON, 0, 0, 0, 0, handle, 0);
		g_VocabularyButton = CreateChild(_T("button"), _T("단어장 편집하기"), NULL, WS_VISIBLE | BS_PUSHBUTTON, 0, 0, 0, 0, handle, 1);
		g_LocalMultiplayButton = CreateChild(_T("button"), _T("로컬 멀티 플레이"), NULL, WS_VISIBLE | BS_PUSHBUTTON, 0, 0, 0, 0, handle, 2);
		g_OnlineMultiplayButton = CreateChild(_T("button"), _T("온라인 멀티 플레이"), NULL, WS_VISIBLE | BS_PUSHBUTTON, 0, 0, 0, 0, handle, 3);

		g_CreateServerButton = CreateChild(_T("button"), _T("서버 만들기"), NULL,  BS_PUSHBUTTON, 0, 0, 0, 0, handle, 4);
		g_JoinServerButton = CreateChild(_T("button"), _T("서버 접속하기"), NULL, BS_PUSHBUTTON, 0, 0, 0, 0, handle, 5);

		g_UpdateButton = CreateChild(_T("button"), _T("새 버전이 있습니다"), NULL, BS_PUSHBUTTON, 0, 0, 0, 0, handle, 6);
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
		UICOMP_CON_N(titleBar, _T("TitleBar"), Horizontal, None, 50, titleFont, titleSection);
		UICOMP_CON(margin1, Horizontal, None, 5, titleSection);
		UICOMP_CON_N(copyrightBar, _T("CopyrightBar"), Horizontal, None, 18, copyrightFont, titleSection);

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
		UICOMP_CON_N(versionBar, _T("VersionBar"), Vertical, None, 150, copyrightFont, versionSection);
		UICOMP_DOH_W(updateButton, &g_UpdateButton, Horizontal, None, 100, updateButtonFont, versionBar);
		return 0;
	}

	case AM_DESTROY:
		DestroyThread(&g_UpdateCheckThread);
		return 0;

	case AM_ACTIVATE:
		SetSceneTitle(handle, NULL);
		return 0;

	case AM_PAINT: {
		START_PAINT;
		SetTextAlign(dc, TA_CENTER);

		UICOMP_FIND(titleBar, _T("TitleBar"));
		DrawString(dc, GetFont(titleBar), GetCenterX(titleBar), GetY(titleBar), CSTR("단어 암기 프로그램"));

		UICOMP_FIND(copyrightBar, _T("CopyrightBar"));
		DrawString(dc, GetFont(copyrightBar), GetCenterX(copyrightBar), GetY(copyrightBar), CSTR("(C) 2020. kmc7468 All rights reserved."));

		UICOMP_FIND(versionBar, _T("VersionBar"));
		DrawString(dc, GetFont(versionBar), GetCenterX(versionBar), GetY(versionBar), g_Version, ARRAYSIZE(g_Version) - 1);

		STOP_PAINT;
	}

	case AM_HASUPDATE:
		ShowWindow(g_UpdateButton, SW_SHOW);
		return 0;

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