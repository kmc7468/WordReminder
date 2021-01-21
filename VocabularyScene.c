#include "Window.h"

#include "UIEngine.h"
#include "Word.h"

static HWND g_WordListStatic;
static HWND g_WordList;

static HWND g_WordStatic, g_WordEdit;

static HWND g_MeaningListStatic;
static HWND g_MeaningList;

static HWND g_MeaningStatic, g_MeaningEdit;
static HWND g_PronunciationStatic, g_PronunciationEdit;
static HWND g_AddMeaningButton, g_RemoveMeaningButton;

static HWND g_RemoveWordButton;

static HWND g_LoadVocabularyButton, g_SaveVocabularyButton;
static Vocabulary g_Vocabulary;
static bool g_IsSaved = true;

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

		g_AddMeaningButton = CreateButton(_T("뜻 추가하기"), WS_VISIBLE, handle, 5);
		g_RemoveMeaningButton = CreateButton(_T("뜻 삭제하기"), WS_VISIBLE, handle, 6);

		g_RemoveWordButton = CreateButton(_T("단어 삭제하기"), WS_VISIBLE, handle, 7);

		g_LoadVocabularyButton = CreateButton(_T("단어장 열기"), WS_VISIBLE, handle, 8);
		g_SaveVocabularyButton = CreateButton(_T("단어장 저장하기"), WS_VISIBLE, handle, 9);
		CreateVocabulary(&g_Vocabulary);
		return 0;

	case AM_CREATEUI: {
		UIEngine* const uiEngine = (UIEngine*)lParam;

		UIFONT_CON(childFont, 18, true);
		UIFONT_CON(itemFont, 18, false);

		UICOMP_DOH(section, Horizontal, None, 100, &uiEngine->RootComponent);
		UIMARG_CON_AS(section, 10);

		UICOMP_CON_W(wordListStatic, &g_WordListStatic, Horizontal, None, 23, childFont, section);
		UIMARG_CON(wordListStatic, Bottom, 5);
		UICOMP_DOW_W(wordList, &g_WordList, Vertical, None, 33, itemFont, section);
		UIMARG_CON(wordList, Right, 5);

		UICOMP_WIN(wordSection, None, section);
		UIMARG_CON(wordSection, Left, 5);

		UICOMP_CON_W(wordStatic, &g_WordStatic, Horizontal, None, 23, childFont, wordSection);
		UIMARG_CON(wordStatic, Bottom, 5);
		UICOMP_CON_W(wordEdit, &g_WordEdit, Horizontal, None, 35, itemFont, wordSection);
		UIMARG_CON(wordEdit, Bottom, 10);

		UICOMP_WIN(meaningSection1, None, wordSection);
		UIMARG_CON(meaningSection1, Bottom, -60);

		UICOMP_CON_W(meaningListStatic, &g_MeaningListStatic, Horizontal, None, 23, childFont, meaningSection1);
		UIMARG_CON(meaningListStatic, Bottom, 5);
		UICOMP_DOW_W(meaningList, &g_MeaningList, Vertical, None, 50, itemFont, meaningSection1);
		UIMARG_CON(meaningList, Right, 5);

		UICOMP_WIN(meaningSection2, None, meaningSection1);
		UIMARG_CON(meaningSection2, Left, 5);
		UIMARG_CON(meaningSection2, Bottom, -60);

		UICOMP_CON_W(meaningStatic, &g_MeaningStatic, Horizontal, None, 23, childFont, meaningSection2);
		UIMARG_CON(meaningStatic, Bottom, 5);
		UICOMP_CON_W(meaningEdit, &g_MeaningEdit, Horizontal, None, 35, itemFont, meaningSection2);
		UIMARG_CON(meaningEdit, Bottom, 10);
		UICOMP_CON_W(pronunciationStatic, &g_PronunciationStatic, Horizontal, None, 23, childFont, meaningSection2);
		UIMARG_CON(pronunciationStatic, Bottom, 5);
		UICOMP_CON_W(pronunciationEdit, &g_PronunciationEdit, Horizontal, None, 35, itemFont, meaningSection2);
		UIMARG_CON(pronunciationEdit, Bottom, 10);
		UICOMP_CON_W(addMeaningButton, &g_AddMeaningButton, Horizontal, None, 55, childFont, meaningSection2);
		UIMARG_CON(addMeaningButton, Bottom, 5);
		UICOMP_CON_W(removeMeaningButton, &g_RemoveMeaningButton, Horizontal, None, 50, childFont, meaningSection2);

		UICOMP_CON_W(removeWordButton, &g_RemoveWordButton, Horizontal, None, 60, childFont, meaningSection1);
		UIMARG_CON(removeWordButton, Left, 5);
		UIMARG_CON(removeWordButton, Top, 10);

		UICOMP_WIN(vocabularyButtonSection, None, wordSection);
		UIMARG_CON(vocabularyButtonSection, Top, 10);

		UICOMP_DOW_W(loadVocabularyButton, &g_LoadVocabularyButton, Vertical, None, 50, childFont, vocabularyButtonSection);
		UIMARG_CON(loadVocabularyButton, Right, 5);
		UICOMP_DOW_W(saveVocabularyButton, &g_SaveVocabularyButton, Vertical, None, 50, childFont, vocabularyButtonSection);
		UIMARG_CON(saveVocabularyButton, Left, 5);
		return 0;
	}

	case AM_ACTIVATE:
		SetSceneTitle(handle, _T("단어장 편집하기"));
		return 0;

	case WM_DESTROY:
		DestroyVocabulary(&g_Vocabulary, true);
		g_IsSaved = true;
		return 0;

	default:
		return DefSubclassProc(handle, message, wParam, lParam);
	}
}