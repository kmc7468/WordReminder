#include "Window.h"

#include "Application.h"
#include "UIEngine.h"

static UIComponent g_UIEngine;

static HFONT g_TitleFont, g_CopyrightFont;
static const TCHAR g_Version[] = _T("v") WR_APPLICATION_VERSION;

static HWND g_SingleplayButton, g_VocabularyButton, g_LocalMultiplayButton, g_OnlineMultiplayButton;
static HFONT g_ButtonFont;

LRESULT CALLBACK MainSceneProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR dummy0, DWORD_PTR dummy1) {
	EVENT {
	case AM_CREATE: {
		CreateUIEngine(&g_UIEngine);
		g_UIEngine.Alignment = Center;

		UICOMP_DOH(area, Horizontal, CenterWithMargin, 85, &g_UIEngine);

		UICOMP_DOC(titleSection, Horizontal, None, area);
		UICOMP_CON_N(titleBar, _T("TitleBar"), Horizontal, None, 50, titleSection);
		UICOMP_CON(margin1, Horizontal, None, 5, titleSection);
		UICOMP_CON_N(copyrightBar, _T("CopyrightBar"), Horizontal, None, 18, titleSection);

		UICOMP_CON(buttonSection1, Horizontal, Center, 230, area);
		UICOMP_CON(buttonSection2, Vertical, None, 300, buttonSection1);
		UICOMP_CON_W(singleplayButton, &g_SingleplayButton, Horizontal, None, 50, buttonSection2);
		UICOMP_CON(buttonMargin1, Horizontal, None, 10, buttonSection2);
		UICOMP_CON_W(vocabularyButton, &g_VocabularyButton, Horizontal, None, 50, buttonSection2);
		UICOMP_CON(buttonMargin2, Horizontal, None, 10, buttonSection2);
		UICOMP_CON_W(localMultiplayButton, &g_LocalMultiplayButton, Horizontal, None, 50, buttonSection2);
		UICOMP_CON(buttonMargin3, Horizontal, None, 10, buttonSection2);
		UICOMP_CON_W(onlineMultiplayButton, &g_OnlineMultiplayButton, Horizontal, None, 50, buttonSection2);

		UICOMP_CON_N(versionBar, _T("VersionBar"), Horizontal, None, 18, area);

		EvaluateUIEngine(&g_UIEngine, handle, WIDTH, HEIGHT);

		g_SingleplayButton = CreateChild(_T("button"), _T("단어 암기하기"), NULL, WS_VISIBLE | BS_PUSHBUTTON, 0, 0, 0, 0, handle, 0);
		g_VocabularyButton = CreateChild(_T("button"), _T("단어장 편집하기"), NULL, WS_VISIBLE | BS_PUSHBUTTON, 0, 0, 0, 0, handle, 1);
		g_LocalMultiplayButton = CreateChild(_T("button"), _T("로컬 멀티 플레이"), NULL, WS_VISIBLE | BS_PUSHBUTTON, 0, 0, 0, 0, handle, 2);
		g_OnlineMultiplayButton = CreateChild(_T("button"), _T("온라인 멀티 플레이"), NULL, WS_VISIBLE | BS_PUSHBUTTON, 0, 0, 0, 0, handle, 3);
		return 0;
	}

	case AM_DESTROY:
		DestroyUIEngine(&g_UIEngine);
		return 0;

	case AM_ACTIVATE:
		SetSceneTitle(handle, NULL);
		return 0;

	case AM_CREATEFONT:
		if (lParam) {
			g_TitleFont = CreateGlobalFont(D(50), true);
			g_CopyrightFont = CreateGlobalFont(D(18), false);

			g_ButtonFont = CreateGlobalFont(D(18), true);
			SetFont(g_SingleplayButton, g_ButtonFont);
			SetFont(g_VocabularyButton, g_ButtonFont);
			SetFont(g_LocalMultiplayButton, g_ButtonFont);
			SetFont(g_OnlineMultiplayButton, g_ButtonFont);
		}
		return 0;

	case AM_DESTROYFONT:
		if (lParam) {
			DeleteObject(g_TitleFont);
			DeleteObject(g_CopyrightFont);

			DeleteObject(g_ButtonFont);
		}
		return 0;

	case WM_SIZE:
		EvaluateUIEngine(&g_UIEngine, handle, WIDTH, HEIGHT);
		UpdateUIEngine(&g_UIEngine);
		return 0;

	case WM_PAINT: {
		START_PAINT;
		SetTextAlign(dc, TA_CENTER);

		UICOMP_FIND(titleBar, _T("TitleBar"));
		DrawString(dc, g_TitleFont, GetCenterX(titleBar), GetY(titleBar), CSTR("단어 암기 프로그램"));

		UICOMP_FIND(copyrightBar, _T("CopyrightBar"));
		DrawString(dc, g_CopyrightFont, GetCenterX(copyrightBar), GetY(copyrightBar), CSTR("(C) 2020. kmc7468 All rights reserved."));

		UICOMP_FIND(versionBar, _T("VersionBar"));
		DrawString(dc, g_CopyrightFont, GetCenterX(versionBar), GetY(versionBar), g_Version, ARRAYSIZE(g_Version) - 1);

		STOP_PAINT;
	}

	default:
		return DefSubclassProc(handle, message, wParam, lParam);
	}
}