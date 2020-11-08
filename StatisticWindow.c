#include "Window.h"

#include "Word.h"

#include <stdlib.h>

static HWND g_WordList;
static HWND g_WordEdit, g_PronunciationEdit, g_MeaningEdit;
static HWND g_AllWordRemindButton, g_WrongWordRemindButton, g_DoneButton;

static Vocabulary g_Vocabulary;
static Vocabulary g_WrongVocabulary;

static bool g_ShouldEnableMainWindow = true;

LRESULT CALLBACK StatisticWindowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam) {
	EVENT {
	case WM_CREATE:
		g_WordList = CreateAndShowChild(_T("listbox"), NULL, GlobalDefaultFont, WS_BORDER | WS_VSCROLL | LBS_NOTIFY,
			10, 40, WIDTH / 3, HEIGHT - 80, handle, 0);

		g_WordEdit = CreateAndShowChild(_T("edit"), NULL, GlobalDefaultFont, WS_BORDER | WS_GROUP | WS_TABSTOP,
			WIDTH / 3 + 20, 65, WIDTH / 3 * 2 - 45, 25, handle, 1);
		g_PronunciationEdit = CreateAndShowChild(_T("edit"), NULL, GlobalDefaultFont, WS_BORDER | WS_GROUP | WS_TABSTOP,
			WIDTH / 3 + 20, 125, WIDTH / 3 * 2 - 45, 25, handle, 2);
		g_MeaningEdit = CreateAndShowChild(_T("edit"), NULL, GlobalDefaultFont, WS_BORDER | WS_GROUP | WS_TABSTOP,
			WIDTH / 3 + 20, 185, WIDTH / 3 * 2 - 45, 25, handle, 3);
		SendMessage(g_WordEdit, EM_SETREADONLY, TRUE, 0);
		SendMessage(g_PronunciationEdit, EM_SETREADONLY, TRUE, 0);
		SendMessage(g_MeaningEdit, EM_SETREADONLY, TRUE, 0);

		g_AllWordRemindButton = CreateAndShowChild(_T("button"), _T("모든 단어 다시 암기하기"), GlobalBoldFont, BS_PUSHBUTTON,
			WIDTH / 4 + 20, HEIGHT - 103, WIDTH / 5 - 25, 50, handle, 4);
		g_WrongWordRemindButton = CreateAndShowChild(_T("button"), _T("틀린 단어만 다시 암기하기"), GlobalBoldFont, BS_PUSHBUTTON,
			WIDTH / 4 * 2 + 1, HEIGHT - 103, WIDTH / 5 - 25, 50, handle, 5);
		g_DoneButton = CreateAndShowChild(_T("button"), _T("창 닫기"), GlobalBoldFont, BS_PUSHBUTTON,
			WIDTH / 4 * 3 + 1, HEIGHT - 103, WIDTH / 5 - 25, 50, handle, 6);
		return 0;

	case WM_DESTROY:
		DestroyVocabulary(&g_Vocabulary);
		DestroyVocabulary(&g_WrongVocabulary);

		if (g_ShouldEnableMainWindow) {
			EnableWindow(MainWindow, TRUE);
			SetWindowPos(MainWindow, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}
		g_ShouldEnableMainWindow = true;
		return 0;

	case WM_SIZE:
		SetWindowPos(g_WordList, HWND_TOP, 0, 0, WIDTH / 3, HEIGHT - 80, SWP_NOMOVE);

		SetWindowPos(g_WordEdit, HWND_TOP, WIDTH / 3 + 20, 65, WIDTH / 3 * 2 - 45, 25, SWP_NOZORDER);
		SetWindowPos(g_PronunciationEdit, HWND_TOP, WIDTH / 3 + 20, 125, WIDTH / 3 * 2 - 45, 25, SWP_NOZORDER);
		SetWindowPos(g_MeaningEdit, HWND_TOP, WIDTH / 3 + 20, 185, WIDTH / 3 * 2 - 45, 25, SWP_NOZORDER);

		SetWindowPos(g_AllWordRemindButton, HWND_TOP, WIDTH / 3 + 20, HEIGHT - 163, WIDTH / 3 - 25, 50, 0);
		SetWindowPos(g_WrongWordRemindButton, HWND_TOP, WIDTH / 3 * 2 + 1, HEIGHT - 163, WIDTH / 3 - 25, 50, 0);
		SetWindowPos(g_DoneButton, HWND_TOP, WIDTH / 3 + 20, HEIGHT - 103, WIDTH / 3 * 2 - 45, 50, 0);
		return 0;

	case WM_PAINT: {
		PAINTSTRUCT ps;
		const HDC dc = BeginPaint(handle, &ps);

		DrawTextUsingFont(dc, GlobalBoldFont, 10, 10, STRING("틀린 단어 목록"));
		DrawTextUsingFont(dc, GlobalDefaultFont, WIDTH / 3 + 20, 40, STRING("단어"));
		DrawTextUsingFont(dc, GlobalDefaultFont, WIDTH / 3 + 20, 100, STRING("발음"));
		DrawTextUsingFont(dc, GlobalDefaultFont, WIDTH / 3 + 20, 160, STRING("뜻"));

		EndPaint(handle, &ps);
		return 0;
	}

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case 0: {
			if (HIWORD(wParam) != LBN_SELCHANGE) break;

			const int index = (int)SendMessage(g_WordList, LB_GETCURSEL, 0, 0);
			if (index == -1) break;

			SetWindowText(g_WordEdit, g_WrongVocabulary.Array[index].Word);
			SetWindowText(g_PronunciationEdit, g_WrongVocabulary.Array[index].Pronunciation);
			SetWindowText(g_MeaningEdit, g_WrongVocabulary.Array[index].Meaning);
			break;
		}

		case 4:
		case 5: {
			Vocabulary* const vocabulary = malloc(sizeof(Vocabulary));
			if (LOWORD(wParam) == 4) {
				*vocabulary = g_Vocabulary;
				g_Vocabulary.Array = NULL;
				g_Vocabulary.Count = 0;
			} else {
				if (GetUniqueWordCount(&g_WrongVocabulary) < 5) {
					MessageBox(handle, _T("적어도 뜻이 다른 단어 5개를 틀려야 합니다."), _T("오류"), MB_OK | MB_ICONERROR);
					free(vocabulary);
					break;
				}

				*vocabulary = g_WrongVocabulary;
				g_WrongVocabulary.Array = NULL;
				g_WrongVocabulary.Count = 0;
			}

			const HWND questionOptionWindow = CreateAndShowWindow(_T("QuestionOptionWindow"), _T("단어 암기하기"), SW_SHOW);
			SendMessage(questionOptionWindow, WM_USER, 0, (LPARAM)vocabulary);
			g_ShouldEnableMainWindow = false;
			SendMessage(handle, WM_CLOSE, 0, 0);
			break;
		}

		case 6:
			SendMessage(handle, WM_CLOSE, 0, 0);
			break;
		}
		return 0;

	case WM_USER:
		g_Vocabulary = *(Vocabulary*)lParam;
		for (int i = 0; i < g_Vocabulary.Count; ++i) {
			if (g_Vocabulary.Array[i].IsWrong) {
				Word word;
				CopyWord(&word, g_Vocabulary.Array + i);
				AddWord(&g_WrongVocabulary, &word);
				SendMessage(g_WordList, LB_ADDSTRING, 0, (LPARAM)word.Word);
			}
		}
		free((void*)lParam);
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