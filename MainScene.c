#include "Window.h"

#define AM_HASUPDATE		AM_USER + 0
#define AM_MERGEBUTTON		AM_USER + 1

#include "Application.h"
#include "Http.h"
#include "UIEngine.h"

#include <stdlib.h>
#include <string.h>

static HWND g_TitleStatic, g_CopyrightStatic, g_VersionStatic;

static HWND g_SingleplayButton, g_VocabularyButton, g_LocalMultiplayButton, g_OnlineMultiplayButton;

static HWND g_CreateServerButton, g_JoinServerButton;
static Thread g_MergeButtonThread;
static DWORD WINAPI MergeButtonThread(LPVOID param);

static HWND g_UpdateButton;
static Thread g_UpdateCheckThread;
static DWORD WINAPI UpdateCheckThread(LPVOID param);

LRESULT CALLBACK MainSceneProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR dummy0, DWORD_PTR dummy1) {
	EVENT {
	case AM_CREATE:
		g_TitleStatic = CreateStatic(_T("단어 암기 프로그램"), WS_VISIBLE | SS_CENTER, handle, -1);
		g_CopyrightStatic = CreateStatic(_T("(C) 2020-2021. kmc7468 All rights reserved."), WS_VISIBLE | SS_CENTER, handle, -1);
		g_VersionStatic = CreateStatic(_T("v") WR_APPLICATION_VERSION, WS_VISIBLE | SS_CENTER, handle, -1);

		g_SingleplayButton = CreateButton(_T("단어 암기하기"), WS_VISIBLE, handle, 0);
		g_VocabularyButton = CreateButton(_T("단어장 편집하기"), WS_VISIBLE, handle, 1);
		g_LocalMultiplayButton = CreateButton(_T("로컬 멀티 플레이"), WS_VISIBLE, handle, 2);
		g_OnlineMultiplayButton = CreateButton(_T("온라인 멀티 플레이"), WS_VISIBLE, handle, 3);

		g_CreateServerButton = CreateButton(_T("서버 만들기"), 0, handle, 4);
		g_JoinServerButton = CreateButton(_T("서버 접속하기"), 0, handle, 5);

		g_UpdateButton = CreateButton(_T("새 버전이 있습니다"), 0, handle, 6);
		StartThread(&g_UpdateCheckThread, UpdateCheckThread, handle);
		return 0;

	case AM_CREATEUI: {
		UIEngine* const uiEngine = (UIEngine*)lParam;

		UIFONT_CON(titleFont, 50, true);
		UIFONT_CON(copyrightFont, 18, false);
		UIFONT_CON(buttonFont, 18, true);
		UIFONT_CON(updateButtonFont, 14, true);

		uiEngine->RootComponent.Alignment = Center;

		UICOMP_DOH(section, Horizontal, CenterWithMargin, 85, &uiEngine->RootComponent);

		UICOMP_DOC(titleSection, Horizontal, None, section);
		UICOMP_CON_W(titleBar, &g_TitleStatic, Horizontal, None, 55, titleFont, titleSection);
		UIMARG_CON(titleBar, Bottom, 5);
		UICOMP_CON_W(copyrightBar, &g_CopyrightStatic, Horizontal, None, 18, copyrightFont, titleSection);

		UICOMP_CON(buttonSection1, Horizontal, Center, 230, section);
		UICOMP_CON(buttonSection2, Vertical, None, 300, buttonSection1);
		UICOMP_CON_W(singleplayButton, &g_SingleplayButton, Horizontal, None, 60, buttonFont, buttonSection2);
		UIMARG_CON(singleplayButton, Bottom, 10);
		UICOMP_CON_W(vocabularyButton, &g_VocabularyButton, Horizontal, None, 60, buttonFont, buttonSection2);
		UIMARG_CON(vocabularyButton, Bottom, 10);
		UICOMP_CON_W(localMultiplayButton, &g_LocalMultiplayButton, Horizontal, None, 60, buttonFont, buttonSection2);
		UIMARG_CON(localMultiplayButton, Bottom, 10);
		UICOMP_CON_W(onlineMultiplayButton, &g_OnlineMultiplayButton, Horizontal, CenterWithMargin, 50, buttonFont, buttonSection2);

		UICOMP_CON_W(createServerButton, &g_CreateServerButton, Vertical, None, 145, buttonFont, onlineMultiplayButton);
		UICOMP_CON_W(joinServerButton, &g_JoinServerButton, Vertical, None, 145, buttonFont, onlineMultiplayButton);

		UICOMP_CON(versionSection, Horizontal, Center, 18, section);
		UICOMP_CON_W(versionBar, &g_VersionStatic, Vertical, None, 150, copyrightFont, versionSection);
		UICOMP_DOH_W(updateButton, &g_UpdateButton, Horizontal, None, 100, updateButtonFont, versionBar);
		return 0;
	}

	case AM_ACTIVATE:
		SetSceneTitle(handle, NULL);
		return 0;

	case WM_DESTROY:
		DestroyThread(&g_MergeButtonThread);
		DestroyThread(&g_UpdateCheckThread);
		return 0;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case 1:
			DestroyWindow(ChangeScene(MainWindow, CreateScene(MainWindow, VocabularySceneProc)));
			break;

		case 3:
			ShowWindow(g_CreateServerButton, SW_SHOW);
			ShowWindow(g_JoinServerButton, SW_SHOW);
			ShowWindow(g_OnlineMultiplayButton, SW_HIDE);

			StartThread(&g_MergeButtonThread, MergeButtonThread, handle);
			break;

		case 6:
			ShellExecute(NULL, _T("open"), WR_APPLICATION_GITHUB_RELEASE, NULL, NULL, SW_SHOWNORMAL);
			break;
		}
		return 0;

	case AM_HASUPDATE:
		ShowWindow(g_VersionStatic, SW_HIDE);
		ShowWindow(g_UpdateButton, SW_SHOW);
		return 0;

	case AM_MERGEBUTTON:
		ShowWindow(g_OnlineMultiplayButton, SW_SHOW);
		ShowWindow(g_CreateServerButton, SW_HIDE);
		ShowWindow(g_JoinServerButton, SW_HIDE);
		return 0;

	default:
		return DefSubclassProc(handle, message, wParam, lParam);
	}
}

DWORD WINAPI MergeButtonThread(LPVOID param) {
	Sleep(5000);
	SendMessage((HWND)param, AM_MERGEBUTTON, 0, 0);
	return 0;
}

DWORD WINAPI UpdateCheckThread(LPVOID param) {
	HttpRequest request;
	if (!CreateHttpRequest(&request, WR_APPLICATION_GITHUB_RELEASE, _T("GET"), true)) return 0;

	if (SendHttpRequest(&request, NULL)) {
		const LPTSTR result = GetHttpResponseHeader(&request, Location);
		if (result) {
			const LPTSTR end = result + _tcslen(result);
			LPTSTR begin = result, temp;
			while (begin < end && (temp = _tcschr(begin, '/'))) {
				begin = temp + 1;
			}

			if (_tcsncmp(begin, WR_APPLICATION_VERSION, ARRAYSIZE(WR_APPLICATION_VERSION) - 1)) {
				Sleep(600);
				SendMessage((HWND)param, AM_HASUPDATE, 0, 0);
			}

			free(result);
		}
	}

	DestroyHttpRequest(&request);
	return 0;
}