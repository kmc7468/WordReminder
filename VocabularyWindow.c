#include "Window.h"

#include "Word.h"

#include <ShlObj.h>

static Vocabulary g_Vocabulary;
static bool g_IsSaved = true;

static HWND g_WordList;
static HWND g_WordEdit, g_PronunciationEdit, g_MeaningEdit;
static HWND g_AddWordButton, g_RemoveWordButton;
static HWND g_LoadVocabularyButton, g_SaveVocabularyButton;

static OPENFILENAME g_FileDialog;
static TCHAR g_FileDialogPath[MAX_PATH];

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

		g_AddWordButton = CreateAndShowChild(_T("button"), _T("�ܾ� �߰�"), GlobalBoldFont, BS_PUSHBUTTON,
			WIDTH / 3 + 20, 220, WIDTH / 3 * 2 - 45, 50, handle, 4);
		g_RemoveWordButton = CreateAndShowChild(_T("button"), _T("�ܾ� ����"), GlobalBoldFont, BS_PUSHBUTTON,
			WIDTH / 3 + 20, 280, WIDTH / 3 * 2 - 45, 50, handle, 5);

		g_LoadVocabularyButton = CreateAndShowChild(_T("button"), _T("�ܾ��� ����"), GlobalBoldFont, BS_PUSHBUTTON,
			WIDTH / 3 + 20, HEIGHT - 103, WIDTH / 3 - 25, 50, handle, 6);
		g_SaveVocabularyButton = CreateAndShowChild(_T("button"), _T("�ܾ��� ����"), GlobalBoldFont, BS_PUSHBUTTON,
			WIDTH / 3 * 2 + 1, HEIGHT - 103, WIDTH / 3 - 25, 50, handle, 7);

		g_FileDialog.hwndOwner = handle;
		g_FileDialog.lpstrDefExt = _T("kwl");
		g_FileDialog.lpstrFile = g_FileDialogPath;
		g_FileDialog.lpstrFilter = _T("�ܾ��� ����(*.kwl)\0*.kwl\0��� ����(*.*)\0*.*\0");
		g_FileDialog.lStructSize = sizeof(g_FileDialog);
		g_FileDialog.nMaxFile = ARRAYSIZE(g_FileDialogPath);

		TCHAR desktop[MAX_PATH];
		SHGetSpecialFolderPath(HWND_DESKTOP, desktop, CSIDL_DESKTOP, FALSE);
		g_FileDialog.lpstrInitialDir = desktop;
		return 0;

	case WM_DESTROY:
		DestroyVocabulary(&g_Vocabulary);
		EnableWindow(MainWindow, TRUE);
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

		DrawTextUsingFont(dc, GlobalBoldFont, 10, 10, STRING("�ܾ� ���"));
		DrawTextUsingFont(dc, GlobalDefaultFont, WIDTH / 3 + 20, 40, STRING("�ܾ�"));
		DrawTextUsingFont(dc, GlobalDefaultFont, WIDTH / 3 + 20, 100, STRING("����"));
		DrawTextUsingFont(dc, GlobalDefaultFont, WIDTH / 3 + 20, 160, STRING("��"));

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
			if (!(word.Word = malloc(sizeof(TCHAR) * 201)) ||
				!(word.Pronunciation = malloc(sizeof(TCHAR) * 201)) ||
				!(word.Meaning = malloc(sizeof(TCHAR) * 201))) {
				MessageBox(handle, _T("�޸𸮰� �����մϴ�."), _T("����"), MB_OK | MB_ICONERROR);
				DestroyWord(&word);
				break;
			}

			GetWindowText(g_WordEdit, word.Word, 201);
			GetWindowText(g_PronunciationEdit, word.Pronunciation, 201);
			GetWindowText(g_MeaningEdit, word.Meaning, 201);
			if (_tcslen(word.Word) == 0 || _tcslen(word.Meaning) == 0) {
				MessageBox(handle, _T("�ܾ�� ���� �� �Է��ؾ� �մϴ�."), _T("����"), MB_OK | MB_ICONERROR);
				DestroyWord(&word);
				break;
			}

			AddWord(&g_Vocabulary, &word);
			g_IsSaved = false;

			SetWindowText(g_WordEdit, _T(""));
			SetWindowText(g_PronunciationEdit, _T(""));
			SetWindowText(g_MeaningEdit, _T(""));
			SendMessage(g_WordList, LB_ADDSTRING, 0, (LPARAM)word.Word);
			break;
		}

		case 5: {
			const int index = (int)SendMessage(g_WordList, LB_GETCURSEL, 0, 0);
			if (index == -1) break;

			g_IsSaved = false;
			SendMessage(g_WordList, LB_DELETESTRING, index, 0);
			RemoveWord(&g_Vocabulary, index);
			break;
		}

		case 6:
			if (!g_IsSaved &&
				MessageBox(handle, _T("�ܾ����� ������� �ʾҽ��ϴ�. �ٸ� �ܾ����� ���� ������� ���� ������ �����˴ϴ�. ���� �ٸ� �ܾ����� ���ðڽ��ϱ�?"), _T("���"), MB_YESNO | MB_ICONQUESTION) != IDYES) break;

			g_FileDialog.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST;
			if (GetOpenFileName(&g_FileDialog)) {
				for (int i = 0; i < g_Vocabulary.Count; ++i) {
					SendMessage(g_WordList, LB_DELETESTRING, 0, 0);
				}
				DestroyVocabulary(&g_Vocabulary);

				LoadVocabulary(&g_Vocabulary, g_FileDialogPath);
				for (int i = 0; i < g_Vocabulary.Count; ++i) {
					SendMessage(g_WordList, LB_ADDSTRING, 0, (LPARAM)g_Vocabulary.Array[i].Word);
				}
				SetWindowText(g_WordEdit, _T(""));
				SetWindowText(g_PronunciationEdit, _T(""));
				SetWindowText(g_MeaningEdit, _T(""));
			}
			break;

		case 7:
			if (GetUniqueWordCount(&g_Vocabulary) < 5) {
				MessageBox(handle, _T("��� ���� �ٸ� �ܾ� 5���� �ܾ��忡 �߰��ؾ� �մϴ�."), _T("����"), MB_OK | MB_ICONERROR);
				break;
			}

			g_FileDialog.Flags = OFN_EXPLORER | OFN_HIDEREADONLY;
			if (GetSaveFileName(&g_FileDialog)) {
				if (SaveVocabulary(&g_Vocabulary, g_FileDialogPath)) {
					g_IsSaved = true;
				} else {
					MessageBox(handle, _T("���� �� �� �� ���� ������ �߻��߽��ϴ�."), _T("����"), MB_OK | MB_ICONERROR);
				}
			}
			break;
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