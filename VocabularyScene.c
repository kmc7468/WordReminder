#include "Window.h"

#define AM_WORDSELCHANGE	AM_USER + 0
#define AM_MEANINGSELCHANGE	AM_USER + 1
#define AM_WORDSUPDATE		AM_USER + 2
#define AM_MEANINGSUPDATE	AM_USER + 3

#include "Application.h"
#include "UIEngine.h"
#include "Word.h"

#include <string.h>

typedef struct {
	Vocabulary Vocabulary;
	Word* SelectedWord;
	Meaning* SelectedMeaning;
	bool IsSaved;
} VocabularyStatus;

static void CreateVocabularyStatus(VocabularyStatus* vocabularyStatus);
static void DestroyVocabularyStatus(VocabularyStatus* vocabularyStatus);

static HWND g_WordListStatic;
static HWND g_WordList;

static HWND g_WordStatic, g_WordEdit;

static HWND g_MeaningListStatic;
static HWND g_MeaningList;

static HWND g_MeaningStatic, g_MeaningEdit;
static HWND g_PronunciationStatic, g_PronunciationEdit;
static HWND g_AddMeaningButton, g_RemoveMeaningButton;
static WNDPROC g_MeaningEditDefWndProc, g_PronunciationEditDefWndProc;
static LRESULT CALLBACK ReturnAwarenessEditSubclassProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);

static HWND g_RemoveWordButton;

static HWND g_MainButton, g_LoadVocabularyButton, g_SaveVocabularyButton;
static VocabularyStatus g_VocabularyStatus;

LRESULT CALLBACK VocabularySceneProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR dummy0, DWORD_PTR dummy1) {
	EVENT {
	case AM_CREATE:
		g_WordListStatic = CreateStatic(_T("단어 목록(0개)"), WS_VISIBLE | SS_LEFT, handle, -1);
		g_WordList = CreateList(WS_VISIBLE, handle, 0);

		g_WordStatic = CreateStatic(_T("단어"), WS_VISIBLE | SS_LEFT, handle, -1);
		g_WordEdit = CreateEdit(WS_VISIBLE | WS_GROUP | WS_TABSTOP, handle, 1);

		g_MeaningListStatic = CreateStatic(_T("뜻 목록(0개)"), WS_VISIBLE | SS_LEFT, handle, -1);
		g_MeaningList = CreateList(WS_VISIBLE, handle, 2);

		g_MeaningStatic = CreateStatic(_T("뜻"), WS_VISIBLE | SS_LEFT, handle, -1);
		g_MeaningEdit = CreateEdit(WS_VISIBLE | WS_GROUP | WS_TABSTOP, handle, 3);
		g_PronunciationStatic = CreateStatic(_T("발음"), WS_VISIBLE | SS_LEFT, handle, -1);
		g_PronunciationEdit = CreateEdit(WS_VISIBLE | WS_GROUP | WS_TABSTOP, handle, 4);
		g_MeaningEditDefWndProc = (WNDPROC)SetWindowLongPtr(g_MeaningEdit, GWLP_WNDPROC, (LONG_PTR)ReturnAwarenessEditSubclassProc);
		g_PronunciationEditDefWndProc = (WNDPROC)SetWindowLongPtr(g_PronunciationEdit, GWLP_WNDPROC, (LONG_PTR)ReturnAwarenessEditSubclassProc);

		g_AddMeaningButton = CreateButton(_T("뜻 추가하기"), WS_VISIBLE, handle, 5);
		g_RemoveMeaningButton = CreateButton(_T("뜻 삭제하기"), WS_VISIBLE, handle, 6);

		g_RemoveWordButton = CreateButton(_T("단어 삭제하기"), WS_VISIBLE, handle, 7);

		g_MainButton = CreateButton(_T("이전으로"), WS_VISIBLE, handle, 8);
		g_LoadVocabularyButton = CreateButton(_T("단어장 열기"), WS_VISIBLE, handle, 9);
		g_SaveVocabularyButton = CreateButton(_T("단어장 저장하기"), WS_VISIBLE, handle, 10);
		CreateVocabularyStatus(&g_VocabularyStatus);
		return 0;

	case AM_CREATEUI: {
		UIEngine* const uiEngine = (UIEngine*)lParam;

		UIFONT_CON(childFont, 18, true);
		UIFONT_CON(itemFont, 18, false);

		UICOMP_DOH(section, Horizontal, None, 100, &uiEngine->RootComponent);
		UIMARG_CON_AS(section, 10);

		UICOMP_WIN(vocabularySection, None, section);
		UIMARG_CON(vocabularySection, Bottom, -60);

		UICOMP_CON_W(wordListStatic, &g_WordListStatic, Horizontal, None, 23, childFont, vocabularySection);
		UIMARG_CON(wordListStatic, Bottom, 5);
		UICOMP_DOW_W(wordList, &g_WordList, Vertical, None, 33, itemFont, vocabularySection);
		UIMARG_CON(wordList, Right, 5);

		UICOMP_WIN(wordSection, None, vocabularySection);
		UIMARG_CON(wordSection, Left, 5);

		UICOMP_CON_W(wordStatic, &g_WordStatic, Horizontal, None, 23, childFont, wordSection);
		UIMARG_CON(wordStatic, Bottom, 5);
		UICOMP_CON_W(wordEdit, &g_WordEdit, Horizontal, None, 35, itemFont, wordSection);
		UIMARG_CON(wordEdit, Bottom, 10);

		UICOMP_CON_W(meaningListStatic, &g_MeaningListStatic, Horizontal, None, 23, childFont, wordSection);
		UIMARG_CON(meaningListStatic, Bottom, 5);
		UICOMP_DOW_W(meaningList, &g_MeaningList, Vertical, None, 50, itemFont, wordSection);
		UIMARG_CON(meaningList, Right, 5);

		UICOMP_WIN(meaningSection, None, wordSection);
		UIMARG_CON(meaningSection, Left, 5);
		UIMARG_CON(meaningSection, Bottom, -60);

		UICOMP_CON_W(meaningStatic, &g_MeaningStatic, Horizontal, None, 23, childFont, meaningSection);
		UIMARG_CON(meaningStatic, Bottom, 5);
		UICOMP_CON_W(meaningEdit, &g_MeaningEdit, Horizontal, None, 35, itemFont, meaningSection);
		UIMARG_CON(meaningEdit, Bottom, 10);
		UICOMP_CON_W(pronunciationStatic, &g_PronunciationStatic, Horizontal, None, 23, childFont, meaningSection);
		UIMARG_CON(pronunciationStatic, Bottom, 5);
		UICOMP_CON_W(pronunciationEdit, &g_PronunciationEdit, Horizontal, None, 35, itemFont, meaningSection);
		UIMARG_CON(pronunciationEdit, Bottom, 10);
		UICOMP_CON_W(addMeaningButton, &g_AddMeaningButton, Horizontal, None, 55, childFont, meaningSection);
		UIMARG_CON(addMeaningButton, Bottom, 5);
		UICOMP_CON_W(removeMeaningButton, &g_RemoveMeaningButton, Horizontal, None, 50, childFont, meaningSection);

		UICOMP_CON_W(removeWordButton, &g_RemoveWordButton, Horizontal, None, 60, childFont, wordSection);
		UIMARG_CON(removeWordButton, Left, 5);
		UIMARG_CON(removeWordButton, Top, 10);

		UICOMP_WIN(vocabularyButtonSection1, None, section);
		UIMARG_CON(vocabularyButtonSection1, Top, 10);

		UICOMP_DOW_W(mainButon, &g_MainButton, Vertical, None, 33, childFont, vocabularyButtonSection1);
		UIMARG_CON(mainButon, Right, 5);

		UICOMP_WIN(vocabularyButtonSection2, None, vocabularyButtonSection1);
		UIMARG_CON(vocabularyButtonSection2, Left, 5);

		UICOMP_DOW_W(loadVocabularyButton, &g_LoadVocabularyButton, Vertical, None, 50, childFont, vocabularyButtonSection2);
		UIMARG_CON(loadVocabularyButton, Right, 5);
		UICOMP_DOW_W(saveVocabularyButton, &g_SaveVocabularyButton, Vertical, None, 50, childFont, vocabularyButtonSection2);
		UIMARG_CON(saveVocabularyButton, Left, 5);
		return 0;
	}

	case AM_ACTIVATE:
		SetSceneTitle(handle, _T("단어장 편집하기"));
		return 0;

	case AM_DESTROY:
		SetWindowLongPtr(g_MeaningEdit, GWLP_WNDPROC, (LONG_PTR)g_MeaningEditDefWndProc);
		SetWindowLongPtr(g_PronunciationEdit, GWLP_WNDPROC, (LONG_PTR)g_PronunciationEditDefWndProc);

		DestroyVocabularyStatus(&g_VocabularyStatus);
		return 0;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case 0: {
			const int index = (int)SendMessage(g_WordList, LB_GETCURSEL, 0, 0);
			if (index == -1 || HIWORD(wParam) != LBN_SELCHANGE) break;

			g_VocabularyStatus.SelectedWord = GetWord(&g_VocabularyStatus.Vocabulary, index);
			SendMessage(handle, AM_WORDSELCHANGE, 0, 0);
			break;
		}

		case 2: {
			const int index = (int)SendMessage(g_MeaningList, LB_GETCURSEL, 0, 0);
			if (index == -1 || HIWORD(wParam) != LBN_SELCHANGE) break;

			g_VocabularyStatus.SelectedMeaning = GetMeaning(g_VocabularyStatus.SelectedWord, index);
			SendMessage(handle, AM_MEANINGSELCHANGE, 0, 0);
			break;
		}

		case 5: {
			const int wordLength = GetWindowTextLength(g_WordEdit);
			const int meaningLength = GetWindowTextLength(g_MeaningEdit);
			const int pronunciationLength = GetWindowTextLength(g_PronunciationEdit);
			if (g_VocabularyStatus.SelectedWord == NULL && wordLength == 0) {
				MessageBox(handle, _T("단어를 입력하거나, 뜻을 추가할 단어를 선택해야 합니다."), _T("오류"), MB_OK | MB_ICONERROR);
				break;
			} else if (meaningLength == 0) {
				MessageBox(handle, _T("뜻을 입력해야 합니다."), _T("오류"), MB_OK | MB_ICONERROR);
				break;
			}

			const LPTSTR word = malloc(sizeof(TCHAR) * (wordLength + 1));
			const LPTSTR meaning = malloc(sizeof(TCHAR) * (meaningLength + 1));
			const LPTSTR pronunciation = malloc(sizeof(TCHAR) * (pronunciationLength + 1));
			GetWindowText(g_WordEdit, word, wordLength + 1);
			GetWindowText(g_MeaningEdit, meaning, meaningLength + 1);
			GetWindowText(g_PronunciationEdit, pronunciation, pronunciationLength + 1);

			if (g_VocabularyStatus.SelectedWord != NULL && (wordLength == 0 || _tcscmp(word, g_VocabularyStatus.SelectedWord->Word) == 0)) {
				free(word);
			} else {
				const int index = FindWord(&g_VocabularyStatus.Vocabulary, word);
				if (index == -1) {
					Word newWord = { 0 };
					CreateWord(&newWord);

					newWord.Word = word;

					AddWord(&g_VocabularyStatus.Vocabulary, &newWord);
					g_VocabularyStatus.SelectedWord = GetWord(&g_VocabularyStatus.Vocabulary, g_VocabularyStatus.Vocabulary.Words.Count - 1);

					SendMessage(g_WordList, LB_ADDSTRING, 0, (LPARAM)word);
					SendMessage(g_WordList, LB_SETCURSEL, g_VocabularyStatus.Vocabulary.Words.Count - 1, 0);
					SendMessage(handle, AM_WORDSELCHANGE, 0, 0);
					SendMessage(handle, AM_WORDSUPDATE, 0, 0);
				} else {
					g_VocabularyStatus.SelectedWord = GetWord(&g_VocabularyStatus.Vocabulary, index);
					free(word);

					SendMessage(g_WordList, LB_SETCURSEL, index, 0);
					SendMessage(handle, AM_WORDSELCHANGE, 0, 0);
				}
			}

			if (FindMeaning(g_VocabularyStatus.SelectedWord, meaning) != -1) {
				free(meaning);
				free(pronunciation);

				MessageBox(handle, _T("이미 단어장에 등록되어 있는 뜻입니다."), _T("오류"), MB_OK | MB_ICONERROR);
				break;
			}

			Meaning	newMeaning = { 0 };
			newMeaning.Word = g_VocabularyStatus.SelectedWord - (Word*)g_VocabularyStatus.Vocabulary.Words.Array;
			newMeaning.Meaning = meaning;
			newMeaning.Pronunciation = pronunciation;
			AddMeaning(g_VocabularyStatus.SelectedWord, &newMeaning);

			SetWindowText(g_WordEdit, NULL);
			SetWindowText(g_MeaningEdit, NULL);
			SetWindowText(g_PronunciationEdit, NULL);

			SendMessage(g_MeaningList, LB_ADDSTRING, 0, (LPARAM)meaning);
			SendMessage(handle, AM_MEANINGSUPDATE, 0, 0);

			g_VocabularyStatus.IsSaved = false;
			SetSceneTitle(handle, _T("단어장 편집하기*"));
			break;
		}

		case 6: {
			int index = (int)SendMessage(g_MeaningList, LB_GETCURSEL, 0, 0);
			if (index == -1) {
				MessageBox(handle, _T("삭제할 뜻을 선택해야 합니다."), _T("오류"), MB_OK | MB_ICONERROR);
				break;
			}

			RemoveMeaning(g_VocabularyStatus.SelectedWord, index);
			g_VocabularyStatus.SelectedMeaning = NULL;

			SendMessage(g_MeaningList, LB_DELETESTRING, index, 0);
			SendMessage(handle, AM_MEANINGSUPDATE, 0, 0);

			if (g_VocabularyStatus.SelectedWord->Meanings.Count == 0) {
				index = (int)SendMessage(g_WordList, LB_GETCURSEL, 0, 0);
				RemoveWord(&g_VocabularyStatus.Vocabulary, index);
				g_VocabularyStatus.SelectedWord = NULL;

				SendMessage(g_WordList, LB_DELETESTRING, index, 0);
				SendMessage(handle, AM_WORDSUPDATE, 0, 0);
			}

			g_VocabularyStatus.IsSaved = false;
			SetSceneTitle(handle, _T("단어장 편집하기*"));
			break;
		}

		case 7: {
			const int count = g_VocabularyStatus.SelectedWord->Meanings.Count;
			for (int i = 0; i < count; ++i) {
				SendMessage(g_MeaningList, LB_SETCURSEL, 0, 0);
				SendMessage(g_RemoveMeaningButton, BM_CLICK, 0, 0);
			}
			break;
		}

		case 8:
			if (!g_VocabularyStatus.IsSaved &&
				MessageBox(handle, _T("변경 사항이 저장되지 않았습니다. 이전 화면으로 돌아가면 저장되지 않은 내용은 모두 삭제됩니다. 정말 이전 화면으로 돌아가시겠습니까?"), _T("경고"), MB_YESNO | MB_ICONWARNING) != IDYES) break;

			DestroyWindow(ChangeScene(MainWindow, CreateScene(MainWindow, MainSceneProc)));
			break;

		case 9: {
			if (!g_VocabularyStatus.IsSaved &&
				MessageBox(handle, _T("변경 사항이 저장되지 않았습니다. 다른 단어장을 열면 저장되지 않은 내용은 모두 삭제됩니다. 정말 다른 단어장을 여시겠습니까?"), _T("경고"), MB_YESNO | MB_ICONWARNING) != IDYES) break;

			const LPCTSTR path = ShowOpenFileDialog(handle);
			if (path) {
				const int meaningCount = (int)SendMessage(g_MeaningList, LB_GETCOUNT, 0, 0);
				for (int i = 0; i < g_VocabularyStatus.Vocabulary.Words.Count; ++i) {
					SendMessage(g_WordList, LB_DELETESTRING, 0, 0);
				}
				for (int i = 0; i < meaningCount; ++i) {
					SendMessage(g_MeaningList, LB_DELETESTRING, 0, 0);
				}
				DestroyVocabularyStatus(&g_VocabularyStatus);
				SetSceneTitle(handle, _T("단어장 편집하기"));
				SendMessage(handle, AM_MEANINGSUPDATE, 0, 0);

				CreateVocabularyStatus(&g_VocabularyStatus);
				if (!LoadVocabulary(&g_VocabularyStatus.Vocabulary, path)) {
					MessageBox(handle, _T("단어장을 읽는 중 오류가 발생했습니다. 올바른 단어장인지 확인해 보십시오."), _T("경고"), MB_OK | MB_ICONERROR);
					break;
				}

				for (int i = 0; i < g_VocabularyStatus.Vocabulary.Words.Count; ++i) {
					SendMessage(g_WordList, LB_ADDSTRING, 0, (LPARAM)GetWord(&g_VocabularyStatus.Vocabulary, i)->Word);
				}
				SetWindowText(g_WordEdit, NULL);
				SetWindowText(g_MeaningEdit, NULL);
				SetWindowText(g_PronunciationEdit, NULL);

				SendMessage(handle, AM_WORDSUPDATE, 0, 0);
			}
			break;
		}

		case 10: {
			if (!IsUsableVocabulary(&g_VocabularyStatus.Vocabulary, GuessMeaning, 0)) {
				MessageBox(handle, _T("다른 단어에는 없는 고유한 뜻을 가진 단어가 적어도 5개 이상 있어야 합니다."), _T("오류"), MB_OK | MB_ICONERROR);
				break;
			}

			const LPCTSTR path = ShowSaveFileDialog(handle);
			if (path) {
				if (SaveVocabulary(&g_VocabularyStatus.Vocabulary, path)) {
					g_VocabularyStatus.IsSaved = true;
					SetSceneTitle(handle, _T("단어장 편집하기"));
				} else {
					MessageBox(handle, _T("단어장 저장 중 오류가 발생했습니다. 다시 시도해 보십시오."), _T("오류"), MB_OK | MB_ICONERROR);
				}
			}
			break;
		}
		}
		return 0;

	case AM_WORDSELCHANGE: {
		SetWindowText(g_WordEdit, g_VocabularyStatus.SelectedWord->Word);
		SetWindowText(g_MeaningEdit, NULL);
		SetWindowText(g_PronunciationEdit, NULL);

		const int meaningCount = (int)SendMessage(g_MeaningList, LB_GETCOUNT, 0, 0);
		for (int i = 0; i < meaningCount; ++i) {
			SendMessage(g_MeaningList, LB_DELETESTRING, 0, 0);
		}
		for (int i = 0; i < g_VocabularyStatus.SelectedWord->Meanings.Count; ++i) {
			SendMessage(g_MeaningList, LB_ADDSTRING, 0, (LPARAM)GetMeaning(g_VocabularyStatus.SelectedWord, i)->Meaning);
		}
		SendMessage(handle, AM_MEANINGSUPDATE, 0, 0);
		return 0;
	}

	case AM_MEANINGSELCHANGE: {
		SetWindowText(g_WordEdit, g_VocabularyStatus.SelectedWord->Word);
		SetWindowText(g_MeaningEdit, g_VocabularyStatus.SelectedMeaning->Meaning);
		SetWindowText(g_PronunciationEdit, g_VocabularyStatus.SelectedMeaning->Pronunciation);
		return 0;
	}

	case AM_WORDSUPDATE: {
		TCHAR text[100] = _T("단어 목록("), count[100];
		_itot(g_VocabularyStatus.Vocabulary.Words.Count, count, 10);
		_tcscat(text, count);
		_tcscat(text, _T("개)"));

		SetWindowText(g_WordListStatic, text);
		return 0;
	}

	case AM_MEANINGSUPDATE: {
		TCHAR text[100] = _T("뜻 목록("), count[100] = _T("0");
		if (g_VocabularyStatus.SelectedWord) {
			_itot(g_VocabularyStatus.SelectedWord->Meanings.Count, count, 10);
		}
		_tcscat(text, count);
		_tcscat(text, _T("개)"));

		SetWindowText(g_MeaningListStatic, text);
		return 0;
	}

	default:
		return DefSubclassProc(handle, message, wParam, lParam);
	}
}

void CreateVocabularyStatus(VocabularyStatus* vocabularyStatus) {
	CreateVocabulary(&vocabularyStatus->Vocabulary);
	vocabularyStatus->IsSaved = true;
}
void DestroyVocabularyStatus(VocabularyStatus* vocabularyStatus) {
	DestroyVocabulary(&vocabularyStatus->Vocabulary, true);
	memset(vocabularyStatus, 0, sizeof(*vocabularyStatus));
}

LRESULT CALLBACK ReturnAwarenessEditSubclassProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_KEYUP:
		switch (wParam) {
		case VK_RETURN:
			SendMessage(g_AddMeaningButton, BM_CLICK, 0, 0);
			break;
		}
	default:
		return CallWindowProc(handle == g_MeaningEdit ? g_MeaningEditDefWndProc : g_PronunciationEditDefWndProc, handle, message, wParam, lParam);
	}
}