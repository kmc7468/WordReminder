#include "Window.h"

static HWND g_ServerAddressEdit, g_ServerPortEdit;
static HWND g_TurnModeButton, g_FixedModeButton;
static HWND g_ExaminerButton, g_ExamineeButton;
static HWND g_Button;

static bool g_ShouldEnableMainWindow = true;
static bool g_IsServerCreation = false;

LRESULT CALLBACK MultiplayStartWindowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam) {
	EVENT {
	case WM_CREATE:
		SetWindowLong(handle, GWL_STYLE, GetWindowLong(handle, GWL_STYLE) & ~WS_SIZEBOX & ~WS_MAXIMIZEBOX);
		SetWindowPos(handle, HWND_TOP, 0, 0, 500, 175, SWP_NOMOVE);

		g_ServerAddressEdit = CreateAndShowChild(_T("edit"), NULL, GlobalDefaultFont, WS_BORDER | WS_GROUP | WS_TABSTOP,
			10, 35, 300, 25, handle, 0);
		g_ServerPortEdit = CreateAndShowChild(_T("edit"), _T("1234"), GlobalDefaultFont, WS_BORDER | WS_GROUP | WS_TABSTOP,
			320, 35, 153, 25, handle, 1);

		g_Button = CreateAndShowChild(_T("button"), _T("접속하기"), GlobalBoldFont, BS_PUSHBUTTON,
			10, 75, 465, 50, handle, 6);
		return 0;

	case WM_DESTROY:
		if (g_ShouldEnableMainWindow) {
			EnableWindow(MainWindow, TRUE);
		}
		g_ShouldEnableMainWindow = true;
		g_IsServerCreation = false;
		return 0;

	case WM_PAINT: {
		PAINTSTRUCT ps;
		const HDC dc = BeginPaint(handle, &ps);

		DrawTextUsingFont(dc, GlobalBoldFont, 10, 10, STRING("서버 주소"));
		DrawTextUsingFont(dc, GlobalBoldFont, 320, 10, STRING("서버 포트"));
		if (g_IsServerCreation) {
			DrawTextUsingFont(dc, GlobalBoldFont, 10, 70, STRING("모드"));
			DrawTextUsingFont(dc, GlobalBoldFont, 10, 130, STRING("역할"));
		}

		EndPaint(handle, &ps);
		return 0;
	}

	case WM_CTLCOLORSTATIC: {
		const HWND btnHandle = (HWND)lParam;
		if (btnHandle == g_TurnModeButton || btnHandle == g_FixedModeButton || btnHandle == g_ExaminerButton || btnHandle == g_ExamineeButton) {
			SetBkMode((HDC)wParam, TRANSPARENT);
		}
		return 0;
	}

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case 0:
			// TODO
			break;
		}
		return 0;

	case WM_USER:
		g_IsServerCreation = true;
		SetWindowPos(handle, HWND_TOP, 0, 0, 500, 300, SWP_NOMOVE);
		SendMessage(g_ServerAddressEdit, EM_SETREADONLY, TRUE, 0);

		g_TurnModeButton = CreateAndShowChild(_T("button"), _T("턴제 모드"), GlobalDefaultFont, BS_AUTORADIOBUTTON,
			10, 95, 90, 15, handle, 2);
		g_FixedModeButton = CreateAndShowChild(_T("button"), _T("역할 고정 모드"), GlobalDefaultFont, BS_AUTORADIOBUTTON,
			110, 95, 120, 15, handle, 3);
		CheckRadioButton(handle, 2, 3, 2);

		g_ExaminerButton = CreateAndShowChild(_T("button"), _T("출제자"), GlobalDefaultFont, BS_AUTORADIOBUTTON,
			10, 155, 70, 15, handle, 4);
		g_ExamineeButton = CreateAndShowChild(_T("button"), _T("응시자"), GlobalDefaultFont, BS_AUTORADIOBUTTON,
			90, 155, 70, 15, handle, 5);
		CheckRadioButton(handle, 4, 5, 4);

		SetWindowText(g_Button, _T("만들기"));
		SetWindowPos(g_Button, HWND_TOP, 10, 200, 0, 0, SWP_NOSIZE);
		return 0;

	case WM_CLOSE:
		DestroyWindow(handle);
		return 0;
	}
	return DefWindowProc(handle, message, wParam, lParam);
}