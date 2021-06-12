#include "Window.h"

#define AM_WORDSELCHANGE	AM_USER + 0
#define AM_MEANINGSELCHANGE	AM_USER + 1
#define AM_MEANINGSUPDATE	AM_USER + 2

#include "UIEngine.h"
#include "Word.h"

typedef struct {
	Vocabulary* OriginalVocabulary;
	Vocabulary* WrongVocabulary;
	Word* SelectedWord;
	Meaning* SelectedMeaning;
} VocabularyStatus;

static void CreateVocabularyStatus(VocabularyStatus* vocabularyStatus, QuestionOption* questionOption);
static void DestroyVocabularyStatus(VocabularyStatus* vocabularyStatus);

static HWND g_WordListStatic;
static HWND g_WordList;

static HWND g_WordStatic, g_WordEdit;

static HWND g_MeaningListStatic;
static HWND g_MeaningList;

static HWND g_MeaningStatic, g_MeaningEdit;
static HWND g_PronunciationStatic, g_PronunciationEdit;

static HWND g_MainButton, g_AllWordRemindButton, g_WrongWordRemindButton;
static VocabularyStatus g_VocabularyStatus;

LRESULT CALLBACK StatisticSceneProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR dummy0, DWORD_PTR dummy1) {
	EVENT {
	case AM_CREATE:
		g_WordListStatic = CreateStatic(_T("틀린 단어 목록(0개)"), WS_VISIBLE | SS_LEFT, handle, -1);
		g_WordList = CreateList(WS_VISIBLE, handle, 0);

		g_WordStatic = CreateStatic(_T("단어"), WS_VISIBLE | SS_LEFT, handle, -1);
		g_WordEdit = CreateEdit(WS_VISIBLE | WS_GROUP | WS_TABSTOP | ES_AUTOHSCROLL, handle, 1);
		SendMessage(g_WordEdit, EM_SETREADONLY, TRUE, 0);

		g_MeaningListStatic = CreateStatic(_T("뜻 목록(0개)"), WS_VISIBLE | SS_LEFT, handle, -1);
		g_MeaningList = CreateList(WS_VISIBLE, handle, 2);

		g_MeaningStatic = CreateStatic(_T("뜻"), WS_VISIBLE | SS_LEFT, handle, -1);
		g_MeaningEdit = CreateEdit(WS_VISIBLE | WS_GROUP | WS_TABSTOP | ES_AUTOHSCROLL, handle, 3);
		g_PronunciationStatic = CreateStatic(_T("발음"), WS_VISIBLE | SS_LEFT, handle, -1);
		g_PronunciationEdit = CreateEdit(WS_VISIBLE | WS_GROUP | WS_TABSTOP | ES_AUTOHSCROLL, handle, 4);
		SendMessage(g_MeaningEdit, EM_SETREADONLY, TRUE, 0);
		SendMessage(g_PronunciationEdit, EM_SETREADONLY, TRUE, 0);

		g_MainButton = CreateButton(_T("메인으로"), WS_VISIBLE, handle, 5);
		g_AllWordRemindButton = CreateButton(_T("모든 단어 다시 암기하기"), WS_VISIBLE, handle, 6);
		g_WrongWordRemindButton = CreateButton(_T("틀린 단어만 다시 암기하기"), WS_VISIBLE, handle, 7);
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

		UICOMP_WIN(statisticButtonSection1, None, section);
		UIMARG_CON(statisticButtonSection1, Top, 10);

		UICOMP_DOW_W(mainButon, &g_MainButton, Vertical, None, 33, childFont, statisticButtonSection1);
		UIMARG_CON(mainButon, Right, 5);

		UICOMP_WIN(statisticButtonSection2, None, statisticButtonSection1);
		UIMARG_CON(statisticButtonSection2, Left, 5);

		UICOMP_DOW_W(allWordRemindButton, &g_AllWordRemindButton, Vertical, None, 50, childFont, statisticButtonSection2);
		UIMARG_CON(allWordRemindButton, Right, 5);
		UICOMP_DOW_W(wrongWordRemindButton, &g_WrongWordRemindButton, Vertical, None, 50, childFont, statisticButtonSection2);
		UIMARG_CON(wrongWordRemindButton, Left, 5);
		return 0;
	}

	case AM_ACTIVATE:
		SetSceneTitle(handle, _T("틀린 단어 확인하기"));
		return 0;

	case AM_DESTROY:
		DestroyVocabularyStatus(&g_VocabularyStatus);
		return 0;

	case AM_DATA: {
		CreateVocabularyStatus(&g_VocabularyStatus, (QuestionOption*)lParam);

		TCHAR text[100] = _T("틀린 단어 목록("), count[100];
		_itot(g_VocabularyStatus.WrongVocabulary->Words.Count, count, 10);
		_tcscat(text, count);
		_tcscat(text, _T("개)"));

		SetWindowText(g_WordListStatic, text);
		return 0;
	}

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case 0: {
			const int index = (int)SendMessage(g_WordList, LB_GETCURSEL, 0, 0);
			if (index == -1 || HIWORD(wParam) != LBN_SELCHANGE) break;

			g_VocabularyStatus.SelectedWord = GetWord(g_VocabularyStatus.WrongVocabulary, index);
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

		case 5:
			DestroyWindow(ChangeScene(MainWindow, CreateScene(MainWindow, MainSceneProc)));
			break;

		case 6:
		case 7: {
			Vocabulary* vocabulary;
			if (LOWORD(wParam) == 6) {
				vocabulary = g_VocabularyStatus.OriginalVocabulary;
				g_VocabularyStatus.OriginalVocabulary = NULL;
			} else {
				if (!IsUsableVocabulary(g_VocabularyStatus.WrongVocabulary, GuessMeaning, 0)) {
					MessageBox(handle, _T("다른 단어에는 없는 고유한 뜻을 가진 단어를 적어도 5개 이상 틀렸어야 합니다."), _T("오류"), MB_OK | MB_ICONERROR);
					break;
				}

				vocabulary = g_VocabularyStatus.WrongVocabulary;
				g_VocabularyStatus.WrongVocabulary = NULL;
			}

			const HWND questionOptionScene = CreateScene(MainWindow, QuestionOptionSceneProc);
			SendMessage(questionOptionScene, AM_DATA, DT_VOCABULARY, (LPARAM)vocabulary);

			DestroyWindow(ChangeScene(MainWindow, questionOptionScene));
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

void CreateVocabularyStatus(VocabularyStatus* vocabularyStatus, QuestionOption* questionOption) {
	vocabularyStatus->OriginalVocabulary = calloc(1, sizeof(Vocabulary));
	*vocabularyStatus->OriginalVocabulary = questionOption->Vocabulary;
	DestroyQuestionOption(questionOption);
	free(questionOption);

	vocabularyStatus->WrongVocabulary = calloc(1, sizeof(Vocabulary));
	CreateVocabulary(vocabularyStatus->WrongVocabulary);
	for (int i = 0; i < vocabularyStatus->OriginalVocabulary->Words.Count; ++i) {
		Word* const word = GetWord(vocabularyStatus->OriginalVocabulary, i);
		if (IsWrong(word)) {
			Word newWord = { 0 };
			CopyWord(&newWord, word);
			for (int j = 0; j < newWord.Meanings.Count; ++j) {
				GetMeaning(&newWord, j)->Word = vocabularyStatus->WrongVocabulary->Words.Count;
			}

			AddWord(vocabularyStatus->WrongVocabulary, &newWord);

			SendMessage(g_WordList, LB_ADDSTRING, 0, (LPARAM)word->Word);
		}
	}
}
void DestroyVocabularyStatus(VocabularyStatus* vocabularyStatus) {
	if (vocabularyStatus->OriginalVocabulary) {
		DestroyVocabulary(vocabularyStatus->OriginalVocabulary, true);
		free(vocabularyStatus->OriginalVocabulary);
	}
	if (vocabularyStatus->WrongVocabulary) {
		DestroyVocabulary(vocabularyStatus->WrongVocabulary, true);
		free(vocabularyStatus->WrongVocabulary);
	}

	memset(vocabularyStatus, 0, sizeof(*vocabularyStatus));
}