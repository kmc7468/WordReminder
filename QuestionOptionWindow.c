#include "Window.h"

#include "Word.h"

#include <stdlib.h>

static HWND g_SelectVocabularyButton;
static HWND g_GuessMeaningButton, g_GuessWordButton;
static HWND g_ShouldGivePronunciationButton;
static HWND g_StartButton;

static LPCTSTR g_VocabularyPath;
static Vocabulary* g_Vocabularary;

static bool g_ShouldEnableMainWindow = true;

LRESULT CALLBACK QuestionOptionWindowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam) {
	EVENT {
	case WM_CREATE:
		g_SelectVocabularyButton = CreateAndShowChild(_T("button"), _T("단어장 선택"), GlobalBoldFont, BS_PUSHBUTTON,
			10, 35, WIDTH - 35, 50, handle, 0);

		g_GuessMeaningButton = CreateAndShowChild(_T("button"), _T("단어 보고 뜻 맞추기"), GlobalDefaultFont, BS_AUTOCHECKBOX,
			10, 130, 150, 30, handle, 1);
		g_GuessWordButton = CreateAndShowChild(_T("button"), _T("뜻 보고 단어 맞추기"), GlobalDefaultFont, BS_AUTOCHECKBOX,
			180, 130, 150, 30, handle, 2);

		g_ShouldGivePronunciationButton = CreateAndShowChild(_T("button"), _T("발음 표시하기"), GlobalDefaultFont, BS_AUTOCHECKBOX,
			10, 195, 200, 30, handle, 3);

		g_StartButton = CreateAndShowChild(_T("button"), _T("암기 시작"), GlobalBoldFont, BS_PUSHBUTTON,
			10, HEIGHT - 100, WIDTH - 35, 50, handle, 4);
		return 0;

	case WM_DESTROY:
		if (g_Vocabularary) {
			DestroyVocabulary(g_Vocabularary);
			free(g_Vocabularary);
		}
		if (g_ShouldEnableMainWindow) {
			EnableWindow(MainWindow, TRUE);
		}
		g_ShouldEnableMainWindow = true;
		return 0;

	case WM_SIZE:
		SetWindowPos(g_SelectVocabularyButton, HWND_TOP, 0, 0, WIDTH - 35, 50, SWP_NOMOVE);
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

	case WM_CTLCOLORSTATIC: {
		const HWND btnHandle = (HWND)lParam;
		if (btnHandle == g_GuessMeaningButton || btnHandle == g_GuessWordButton || btnHandle == g_ShouldGivePronunciationButton) {
			SetBkMode((HDC)wParam, TRANSPARENT);
		}
		return 0;
	}

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case 0: {
			const LPCTSTR path = ShowOpenFileDialog(handle);
			if (path) {
				SetWindowText(g_SelectVocabularyButton, _T("단어장 선택됨"));
				g_VocabularyPath = path;

				if (g_Vocabularary) {
					DestroyVocabulary(g_Vocabularary);
					free(g_Vocabularary);
					g_Vocabularary = NULL;
				}
			}
			break;
		}

		case 4: {
			if (!g_VocabularyPath && !g_Vocabularary) {
				MessageBox(handle, _T("단어장을 선택해 주세요."), _T("오류"), MB_OK | MB_ICONERROR);
				break;
			}

			const bool guessingMeaning = (bool)SendMessage(g_GuessMeaningButton, BM_GETCHECK, 0, 0);
			const bool guessingWord = (bool)SendMessage(g_GuessWordButton, BM_GETCHECK, 0, 0);
			if (!guessingMeaning && !guessingWord) {
				MessageBox(handle, _T("문제 유형을 선택해 주세요."), _T("오류"), MB_OK | MB_ICONERROR);
				break;
			}

			QuestionOption* option = calloc(1, sizeof(QuestionOption));
			if (!option) {
				MessageBox(handle, _T("메모리가 부족합니다."), _T("오류"), MB_OK | MB_ICONERROR);
				break;
			}

			if (g_Vocabularary) {
				option->Vocabulary = *g_Vocabularary;
				free(g_Vocabularary);
				g_Vocabularary = NULL;
			} else if (!LoadVocabulary(&option->Vocabulary, g_VocabularyPath)) {
				MessageBox(handle, _T("단어장을 읽는데 실패했습니다. 올바른 단어장인지 확인해 보세요."), _T("오류"), MB_OK | MB_ICONERROR);
				break;
			}
			option->QuestionType |= guessingMeaning ? GuessingMeaning : 0;
			option->QuestionType |= guessingWord ? GuessingWord : 0;
			option->ShouldGivePronunciation = (bool)SendMessage(g_ShouldGivePronunciationButton, BM_GETCHECK, 0, 0);

			const HWND questionWindow = CreateAndShowWindow(_T("QuestionWindow"), _T("단어 암기하기"), SW_SHOW);
			g_ShouldEnableMainWindow = false;
			SendMessage(questionWindow, WM_USER, 0, (LPARAM)option);
			SendMessage(handle, WM_CLOSE, 0, 0);
			break;
		}
		}
		return 0;

	case WM_USER:
		g_Vocabularary = (Vocabulary*)lParam;
		for (int i = 0; i < g_Vocabularary->Count; ++i) {
			g_Vocabularary->Array[i].IsWrong = false;
		}
		SetWindowText(g_SelectVocabularyButton, _T("단어장 선택됨"));
		break;

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