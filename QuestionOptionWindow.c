#include "Window.h"

#include "Word.h"

static HWND g_SelectingVocabularyButton;
static HWND g_GuessingMeaningButton, g_GuessingWordButton;
static HWND g_ShouldGivePronunciationButton;
static HWND g_StartButton;

static LPCTSTR g_VocabularyPath;

static bool g_ShouldEnableMainWindow = true;

LRESULT CALLBACK QuestionOptionWindowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam) {
	EVENT {
	case WM_CREATE:
		g_SelectingVocabularyButton = CreateAndShowChild(_T("button"), _T("단어장 선택"), GlobalBoldFont, BS_PUSHBUTTON,
			10, 35, WIDTH - 35, 50, handle, 0);

		g_GuessingMeaningButton = CreateAndShowChild(_T("button"), _T("단어 보고 뜻 맞추기"), GlobalDefaultFont, BS_AUTOCHECKBOX,
			10, 130, 200, 30, handle, 1);
		g_GuessingWordButton = CreateAndShowChild(_T("button"), _T("뜻 보고 단어 맞추기"), GlobalDefaultFont, BS_AUTOCHECKBOX,
			220, 130, 200, 30, handle, 2);

		g_ShouldGivePronunciationButton = CreateAndShowChild(_T("button"), _T("발음 표시하기"), GlobalDefaultFont, BS_AUTOCHECKBOX,
			10, 195, 200, 30, handle, 3);

		g_StartButton = CreateAndShowChild(_T("button"), _T("암기 시작"), GlobalBoldFont, BS_PUSHBUTTON,
			10, HEIGHT - 100, WIDTH - 35, 50, handle, 4);
		return 0;

	case WM_DESTROY:
		if (g_ShouldEnableMainWindow) {
			EnableWindow(MainWindow, TRUE);
		}
		g_ShouldEnableMainWindow = true;
		return 0;

	case WM_SIZE:
		SetWindowPos(g_SelectingVocabularyButton, HWND_TOP, 0, 0, WIDTH - 35, 50, SWP_NOMOVE);
		SetWindowPos(g_StartButton, HWND_TOP, 10, HEIGHT - 100, WIDTH - 35, 50, 0);
		return 0;

	case WM_PAINT: {
		PAINTSTRUCT ps;
		const HDC dc = BeginPaint(handle, &ps);

		DrawTextUsingFont(dc, GlobalBoldFont, 10, 10, STRING("단어장 선택"));
		DrawTextUsingFont(dc, GlobalBoldFont, 10, 105, STRING("문제 유형 선택"));
		DrawTextUsingFont(dc, GlobalBoldFont, 10, 170, STRING("기타 옵션"));

		EndPaint(handle, &ps);
		return 0;
	}

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case 0: {
			const LPCTSTR path = ShowOpenFileDialog(handle);
			if (path) {
				SetWindowText(g_SelectingVocabularyButton, _T("단어장 선택됨"));
				g_VocabularyPath = path;
			}
			break;
		}

		case 4: {
			if (!g_VocabularyPath) {
				MessageBox(handle, _T("단어장을 선택해 주세요."), _T("오류"), MB_OK | MB_ICONERROR);
				break;
			}

			const bool guessingMeaning = (bool)SendMessage(g_GuessingMeaningButton, BM_GETCHECK, 0, 0);
			const bool guessingWord = (bool)SendMessage(g_GuessingWordButton, BM_GETCHECK, 0, 0);
			if (!guessingMeaning && !guessingWord) {
				MessageBox(handle, _T("문제 유형을 선택해 주세요."), _T("오류"), MB_OK | MB_ICONERROR);
				break;
			}

			QuestionOption* option = malloc(sizeof(QuestionOption));
			if (!option) {
				MessageBox(handle, _T("메모리가 부족합니다."), _T("오류"), MB_OK | MB_ICONERROR);
				break;
			} else if (!LoadVocabulary(&option->Vocabulary, g_VocabularyPath)) {
				MessageBox(handle, _T("단어장을 읽는데 실패했습니다. 올바른 단어장인지 확인해 보세요."), _T("오류"), MB_OK | MB_ICONERROR);
				break;
			}
			option->QuestionType |= guessingMeaning ? GuessingMeaning : 0;
			option->QuestionType |= guessingWord ? GuessingWord : 0;
			option->ShouldGivePronunciation = (bool)SendMessage(g_ShouldGivePronunciationButton, BM_GETCHECK, 0, 0);

			const HWND questionWindow = CreateAndShowWindow(_T("QuestionWindow"), _T("단어 암기하기"), SW_SHOW);
			SendMessage(questionWindow, WM_USER, 0, (LPARAM)option);
			SendMessage(handle, WM_CLOSE, 0, 0);
			break;
		}
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
		return 0;
	}
	return DefWindowProc(handle, message, wParam, lParam);
}