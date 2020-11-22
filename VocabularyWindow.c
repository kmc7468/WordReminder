#include "Window.h"

#include "Word.h"

static HWND g_WordList;
static HWND g_WordEdit, g_PronunciationEdit, g_MeaningEdit;
static HWND g_AddWordButton, g_RemoveWordButton;
static HWND g_LoadVocabularyButton, g_SaveVocabularyButton;

static Vocabulary g_Vocabulary;
static bool g_IsSaved = true;

LRESULT CALLBACK VocabularyWindowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam) {
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

		g_AddWordButton = CreateAndShowChild(_T("button"), _T("단어 추가"), GlobalBoldFont, BS_PUSHBUTTON,
			WIDTH / 3 + 20, 220, WIDTH / 3 * 2 - 45, 50, handle, 4);
		g_RemoveWordButton = CreateAndShowChild(_T("button"), _T("단어 삭제"), GlobalBoldFont, BS_PUSHBUTTON,
			WIDTH / 3 + 20, 280, WIDTH / 3 * 2 - 45, 50, handle, 5);

		g_LoadVocabularyButton = CreateAndShowChild(_T("button"), _T("단어장 열기"), GlobalBoldFont, BS_PUSHBUTTON,
			WIDTH / 3 + 20, HEIGHT - 103, WIDTH / 3 - 25, 50, handle, 6);
		g_SaveVocabularyButton = CreateAndShowChild(_T("button"), _T("단어장 저장"), GlobalBoldFont, BS_PUSHBUTTON,
			WIDTH / 3 * 2 + 1, HEIGHT - 103, WIDTH / 3 - 25, 50, handle, 7);
		return 0;

	case WM_DESTROY:
		DestroyVocabulary(&g_Vocabulary);
		g_IsSaved = true;

		EnableWindow(MainWindow, TRUE);
		SetWindowPos(MainWindow, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		return 0;

	case WM_SIZE:
		SetWindowPos(g_WordList, HWND_TOP, 0, 0, WIDTH / 3, HEIGHT - 80, SWP_NOMOVE);

		SetWindowPos(g_WordEdit, 0, WIDTH / 3 + 20, 65, WIDTH / 3 * 2 - 45, 25, SWP_NOZORDER);
		SetWindowPos(g_PronunciationEdit, 0, WIDTH / 3 + 20, 125, WIDTH / 3 * 2 - 45, 25, SWP_NOZORDER);
		SetWindowPos(g_MeaningEdit, 0, WIDTH / 3 + 20, 185, WIDTH / 3 * 2 - 45, 25, SWP_NOZORDER);

		SetWindowPos(g_AddWordButton, HWND_TOP, WIDTH / 3 + 20, 220, WIDTH / 3 * 2 - 45, 50, 0);
		SetWindowPos(g_RemoveWordButton, HWND_TOP, WIDTH / 3 + 20, 280, WIDTH / 3 * 2 - 45, 50, 0);
		SetWindowPos(g_LoadVocabularyButton, HWND_TOP, WIDTH / 3 + 20, HEIGHT - 103, WIDTH / 3 - 25, 50, 0);
		SetWindowPos(g_SaveVocabularyButton, HWND_TOP, WIDTH / 3 * 2 + 1, HEIGHT - 103, WIDTH / 3 - 25, 50, 0);
		return 0;

	case WM_PAINT: {
		PAINTSTRUCT ps;
		const HDC dc = BeginPaint(handle, &ps);

		DrawTextUsingFont(dc, GlobalBoldFont, 10, 10, STRING("단어 목록"));
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

			SetWindowText(g_WordEdit, g_Vocabulary.Array[index].Word);
			SetWindowText(g_PronunciationEdit, g_Vocabulary.Array[index].Pronunciation);
			SetWindowText(g_MeaningEdit, g_Vocabulary.Array[index].Meaning);
			break;
		}

		case 4: {
			Word word = { 0 };
			GetWindowText(g_WordEdit, word.Word = malloc(sizeof(TCHAR) * (GetWindowTextLength(g_WordEdit) + 1)), 201);
			GetWindowText(g_PronunciationEdit, word.Pronunciation = malloc(sizeof(TCHAR) * (GetWindowTextLength(g_PronunciationEdit) + 1)), 201);
			GetWindowText(g_MeaningEdit, word.Meaning = malloc(sizeof(TCHAR) * (GetWindowTextLength(g_MeaningEdit) + 1)), 201);
			if (_tcslen(word.Word) == 0 || _tcslen(word.Meaning) == 0) {
				MessageBox(handle, _T("단어와 뜻은 꼭 입력해야 합니다."), _T("오류"), MB_OK | MB_ICONERROR);
				DestroyWord(&word);
				break;
			}
			AddWord(&g_Vocabulary, &word);
			g_IsSaved = false;

			SetWindowText(g_WordEdit, NULL);
			SetWindowText(g_PronunciationEdit, NULL);
			SetWindowText(g_MeaningEdit, NULL);
			SendMessage(g_WordList, LB_ADDSTRING, 0, (LPARAM)word.Word);
			break;
		}

		case 5: {
			const int index = (int)SendMessage(g_WordList, LB_GETCURSEL, 0, 0);
			if (index == -1) break;

			g_IsSaved = false;
			RemoveWord(&g_Vocabulary, index);
			SendMessage(g_WordList, LB_DELETESTRING, index, 0);
			break;
		}

		case 6: {
			if (!g_IsSaved &&
				MessageBox(handle, _T("단어장이 저장되지 않았습니다. 다른 단어장을 열면 저장되지 않은 내용은 삭제됩니다. 정말 다른 단어장을 여시겠습니까?"), _T("경고"), MB_YESNO | MB_ICONWARNING) != IDYES) break;

			const LPCTSTR path = ShowOpenFileDialog(handle);
			if (path) {
				for (int i = 0; i < g_Vocabulary.Count; ++i) {
					SendMessage(g_WordList, LB_DELETESTRING, 0, 0);
				}
				DestroyVocabulary(&g_Vocabulary);

				LoadVocabulary(&g_Vocabulary, path);
				for (int i = 0; i < g_Vocabulary.Count; ++i) {
					SendMessage(g_WordList, LB_ADDSTRING, 0, (LPARAM)g_Vocabulary.Array[i].Word);
				}
				SetWindowText(g_WordEdit, NULL);
				SetWindowText(g_PronunciationEdit, NULL);
				SetWindowText(g_MeaningEdit, NULL);
			}
			break;
		}

		case 7: {
			if (GetUniqueWordCount(&g_Vocabulary) < 5) {
				MessageBox(handle, _T("적어도 뜻이 다른 단어 5개를 단어장에 추가해야 합니다."), _T("오류"), MB_OK | MB_ICONERROR);
				break;
			}

			const LPCTSTR path = ShowSaveFileDialog(handle);
			if (path) {
				if (SaveVocabulary(&g_Vocabulary, path)) {
					g_IsSaved = true;
				} else {
					MessageBox(handle, _T("저장 중 오류가 발생했습니다."), _T("오류"), MB_OK | MB_ICONERROR);
				}
			}
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