#include "Window.h"

#include "Multiplay.h"
#include "Version.h"
#include "Word.h"

#include <stdlib.h>

static HWND g_SelectVocabularyButton;
static HWND g_GuessMeaningButton, g_GuessWordButton;
static HWND g_GivePronunciationButton;
static HWND g_StartButton;

static LPCTSTR g_VocabularyPath;
static Vocabulary* g_Vocabularary;
static OnlineMultiplayOption* g_MultiplayOption;
static bool g_IsLocalMultiplay;

static bool g_ShouldEnableMainWindow = true;

LRESULT CALLBACK QuestionOptionWindowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam) {
	EVENT {
	case WM_CREATE:
		SetWindowLong(handle, GWL_STYLE, GetWindowLong(handle, GWL_STYLE) & ~WS_SIZEBOX & ~WS_MAXIMIZEBOX);
		SetWindowPos(handle, HWND_TOP, 0, 0, 500, 325, SWP_NOMOVE);

		g_SelectVocabularyButton = CreateAndShowChild(_T("button"), _T("�ܾ��� �����ϱ�"), GlobalBoldFont, BS_PUSHBUTTON,
			10, 35, 465, 50, handle, 0);

		g_GuessMeaningButton = CreateAndShowChild(_T("button"), _T("�ܾ� ���� �� ���߱�"), GlobalDefaultFont, BS_AUTOCHECKBOX,
			10, 130, 150, 15, handle, 1);
		g_GuessWordButton = CreateAndShowChild(_T("button"), _T("�� ���� �ܾ� ���߱�"), GlobalDefaultFont, BS_AUTOCHECKBOX,
			180, 130, 150, 15, handle, 2);
		SendMessage(g_GuessMeaningButton, BM_SETCHECK, !!Setting.GuessMeaning, 0);
		SendMessage(g_GuessWordButton, BM_SETCHECK, !!Setting.GuessWord, 0);

		g_GivePronunciationButton = CreateAndShowChild(_T("button"), _T("���� ǥ���ϱ�"), GlobalDefaultFont, BS_AUTOCHECKBOX,
			10, 190, 200, 15, handle, 3);
		SendMessage(g_GivePronunciationButton, BM_SETCHECK, !!Setting.GivePronunciation, 0);

		g_StartButton = CreateAndShowChild(_T("button"), _T("�����ϱ�"), GlobalBoldFont, BS_PUSHBUTTON,
			10, 225, 465, 50, handle, 4);
		return 0;

	case WM_DESTROY:
		g_VocabularyPath = NULL;
		if (g_Vocabularary) {
			DestroyVocabulary(g_Vocabularary);
			free(g_Vocabularary);
			g_Vocabularary = NULL;
		}
		if (g_MultiplayOption) {
			free(g_MultiplayOption);
			g_MultiplayOption = NULL;
		}
		g_IsLocalMultiplay = false;

		if (g_ShouldEnableMainWindow) {
			EnableWindow(MainWindow, TRUE);
			SetWindowPos(MainWindow, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}
		g_ShouldEnableMainWindow = true;
		return 0;

	case WM_PAINT: {
		PAINTSTRUCT ps;
		const HDC dc = BeginPaint(handle, &ps);

		DrawTextUsingFont(dc, GlobalBoldFont, 10, 10, STRING("�ܾ���"));
		DrawTextUsingFont(dc, GlobalBoldFont, 10, 105, STRING("���� ����"));
		DrawTextUsingFont(dc, GlobalBoldFont, 10, 165, STRING("��Ÿ �ɼ�"));

		EndPaint(handle, &ps);
		return 0;
	}

	case WM_CTLCOLORSTATIC: {
		const HWND btnHandle = (HWND)lParam;
		if (btnHandle == g_GuessMeaningButton || btnHandle == g_GuessWordButton ||
			btnHandle == g_GivePronunciationButton) {
			SetBkMode((HDC)wParam, TRANSPARENT);
		}
		return 0;
	}

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case 0: {
			const LPCTSTR path = ShowOpenFileDialog(handle);
			if (path) {
				SetWindowText(g_SelectVocabularyButton, _T("�ܾ��� ���õ�"));
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
			const bool guessMeaning = (bool)IsDlgButtonChecked(handle, 1);
			const bool guessWord = (bool)IsDlgButtonChecked(handle, 2);
			if (!g_VocabularyPath && !g_Vocabularary) {
				MessageBox(handle, _T("�ܾ����� ������ �ּ���."), _T("����"), MB_OK | MB_ICONERROR);
				break;
			} else if (!guessMeaning && !guessWord) {
				MessageBox(handle, _T("���� ������ ������ �ּ���."), _T("����"), MB_OK | MB_ICONERROR);
				break;
			}

			QuestionOption* option = calloc(1, sizeof(QuestionOption));
			if (g_Vocabularary) {
				option->Vocabulary = *g_Vocabularary;
				free(g_Vocabularary);
				g_Vocabularary = NULL;
			} else if (!LoadVocabulary(&option->Vocabulary, g_VocabularyPath)) {
				MessageBox(handle, _T("�ܾ����� �д� �� ������ �߻��߽��ϴ�. �ùٸ� �ܾ������� Ȯ���� ������."), _T("����"), MB_OK | MB_ICONERROR);
				break;
			}
			option->QuestionType |= guessMeaning ? GuessMeaning : 0;
			option->QuestionType |= guessWord ? GuessWord : 0;
			option->GivePronunciation = (bool)IsDlgButtonChecked(handle, 3);

			if (!g_MultiplayOption || g_MultiplayOption->Mode == TurnMode || g_MultiplayOption->Role == Examinee) {
				const HWND questionWindow = CreateAndShowWindow(_T("QuestionWindow"), _T("�ܾ� �ϱ��ϱ�"), SW_SHOW);
				SendMessage(questionWindow, WM_USER, 0, (LPARAM)option);
				if (g_MultiplayOption) {
					SetWindowText(questionWindow, _T("��Ƽ �÷���"));
					SendMessage(questionWindow, WM_USER + 1, 0, (LPARAM)g_MultiplayOption);
				}
			} else {
				const HWND examinerWindow = CreateAndShowWindow(_T("ExaminerWindow"), _T("��Ƽ �÷���"), SW_SHOW);
				SendMessage(examinerWindow, WM_USER, 0, (LPARAM)option);
				SendMessage(examinerWindow, WM_USER + 1, 0, (LPARAM)g_MultiplayOption);
			}
			g_MultiplayOption = NULL;

			Setting.GuessMeaning = (bool)IsDlgButtonChecked(handle, 1);
			Setting.GuessWord = (bool)IsDlgButtonChecked(handle, 2);
			Setting.GivePronunciation = (bool)IsDlgButtonChecked(handle, 3);

			g_ShouldEnableMainWindow = false;
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
		SetWindowText(g_SelectVocabularyButton, _T("�ܾ��� ���õ�"));
		return 0;

	case WM_USER + 1:
		g_MultiplayOption = (OnlineMultiplayOption*)lParam;
		SetWindowText(g_StartButton, _T("���� �����"));
		return 0;

	case WM_USER + 2:
		g_IsLocalMultiplay = true;
		return 0;

	case WM_CLOSE:
		DestroyWindow(handle);
		return 0;
	}
	return DefWindowProc(handle, message, wParam, lParam);
}