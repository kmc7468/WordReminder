#include "Window.h"

#include "Application.h"
#include "UIEngine.h"
#include "Word.h"

static HWND g_SelectVocabularyStatic, g_SelectVocabularyButton;
static LPCTSTR g_VocabularyPath;

static HWND g_QuestionTypeStatic;
static HWND g_GuessMeaningCheckBox, g_GuessMeaningWithPronunciationCheckBox;
static HWND g_GuessWordCheckBox, g_GuessWordWithPronunciationCheckBox;
static HWND g_GuessPronunciationCheckBox, g_GuessPronunciationWithMeaningCheckBox, g_GroupGuessingMeaningAndPronunciationCheckBox;
static void UpdateQuestionTypeCheckBoxVisibility();

static HWND g_OtherOptionStatic;
static HWND g_ExcludeDuplicatedAnswerCheckBox;

static HWND g_MainButton, g_StartButton;

LRESULT CALLBACK QuestionOptionSceneProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR dummy0, DWORD_PTR dummy1) {
	EVENT {
	case AM_CREATE:
		g_SelectVocabularyStatic = CreateStatic(_T("암기할 단어장"), WS_VISIBLE | SS_LEFT, handle, -1);
		g_SelectVocabularyButton = CreateButton(_T("단어장 선택하기"), WS_VISIBLE, handle, 0);

		g_QuestionTypeStatic = CreateStatic(_T("문제 유형"), WS_VISIBLE | SS_LEFT, handle, -1);
		g_GuessMeaningCheckBox = CreateCheckBox(_T("단어 보고 뜻 맞히기"), WS_VISIBLE, handle, 1);
		g_GuessMeaningWithPronunciationCheckBox = CreateCheckBox(_T("발음 표시하기"), 0, handle, 2);
		g_GuessWordCheckBox = CreateCheckBox(_T("뜻 보고 단어 맞히기"), WS_VISIBLE, handle, 3);
		g_GuessWordWithPronunciationCheckBox = CreateCheckBox(_T("발음 표시하기"), 0, handle, 4);
		g_GuessPronunciationCheckBox = CreateCheckBox(_T("단어 보고 발음 맞히기"), WS_VISIBLE, handle, 5);
		g_GuessPronunciationWithMeaningCheckBox = CreateCheckBox(_T("뜻 표시하기"), 0, handle, 6);
		g_GroupGuessingMeaningAndPronunciationCheckBox = CreateCheckBox(_T("뜻 문제와 발음 문제 묶기"), 0, handle, 7);

		g_OtherOptionStatic = CreateStatic(_T("기타 옵션"), WS_VISIBLE | SS_LEFT, handle, -1);
		g_ExcludeDuplicatedAnswerCheckBox = CreateCheckBox(_T("중복된 문제 제외하기"), WS_VISIBLE, handle, 8);

		g_MainButton = CreateButton(_T("이전으로"), WS_VISIBLE, handle, 9);
		g_StartButton = CreateButton(_T("시작하기"), WS_VISIBLE, handle, 10);

		SendMessage(g_GuessMeaningCheckBox, BM_SETCHECK, Setting.GuessMeaning, 0);
		SendMessage(g_GuessMeaningWithPronunciationCheckBox, BM_SETCHECK, Setting.GuessMeaningWithPronunciation, 0);
		SendMessage(g_GuessWordCheckBox, BM_SETCHECK, Setting.GuessWord, 0);
		SendMessage(g_GuessWordWithPronunciationCheckBox, BM_SETCHECK, Setting.GuessWordWithPronunciation, 0);
		SendMessage(g_GuessPronunciationCheckBox, BM_SETCHECK, Setting.GuessPronunciation, 0);
		SendMessage(g_GuessPronunciationWithMeaningCheckBox, BM_SETCHECK, Setting.GuessPronunciationWithMeaning, 0);
		SendMessage(g_GroupGuessingMeaningAndPronunciationCheckBox, BM_SETCHECK, Setting.GroupGuessingMeaningAndPronunciation, 0);
		UpdateQuestionTypeCheckBoxVisibility();

		SendMessage(g_ExcludeDuplicatedAnswerCheckBox, BM_SETCHECK, Setting.ExcludeDuplicatedAnswer, 0);
		return 0;

	case AM_CREATEUI: {
		UIEngine* const uiEngine = (UIEngine*)lParam;

		UIFONT_CON(buttonFont, 18, true);
		UIFONT_CON(checkBoxFont, 18, false);

		UICOMP_DOH(section1, Horizontal, None, 100, &uiEngine->RootComponent);
		UIMARG_CON_AS(section1, 10);

		UICOMP_WIN(section2, None, section1);
		UIMARG_CON(section2, Bottom, -60);

		UICOMP_CON_W(selectVocabularyStatic, &g_SelectVocabularyStatic, Horizontal, None, 23, buttonFont, section2);
		UIMARG_CON(selectVocabularyStatic, Bottom, 5);
		UICOMP_CON_W(selectVocabularyButton, &g_SelectVocabularyButton, Horizontal, None, 60, buttonFont, section2);
		UIMARG_CON(selectVocabularyButton, Bottom, 10);

		UICOMP_CON_W(questionTypeStatic, &g_QuestionTypeStatic, Horizontal, None, 23, buttonFont, section2);
		UIMARG_CON(questionTypeStatic, Bottom, 5);

		UICOMP_CON(questionTypeSection1, Horizontal, None, 80, section2);
		UICOMP_DOW(questionTypeSection2, Vertical, None, 33.35f, questionTypeSection1);
		UICOMP_DOW(questionTypeSection3, Vertical, None, 66.65f, questionTypeSection1);

		UICOMP_CON_W(guessMeaningCheckBox, &g_GuessMeaningCheckBox, Horizontal, None, 25, checkBoxFont, questionTypeSection2);
		UIMARG_CON(guessMeaningCheckBox, Bottom, 10);
		UICOMP_CON_W(guessMeaningCheckBoxWithPronunciationCheckBox, &g_GuessMeaningWithPronunciationCheckBox, Horizontal, None, 25, checkBoxFont, questionTypeSection3);
		UIMARG_CON(guessMeaningCheckBoxWithPronunciationCheckBox, Bottom, 10);
		UICOMP_CON_W(guessWordCheckBox, &g_GuessWordCheckBox, Horizontal, None, 25, checkBoxFont, questionTypeSection2);
		UIMARG_CON(guessWordCheckBox, Bottom, 10);
		UICOMP_CON_W(guessWordCheckBoxWithPronunciationCheckBox, &g_GuessWordWithPronunciationCheckBox, Horizontal, None, 25, checkBoxFont, questionTypeSection3);
		UIMARG_CON(guessWordCheckBoxWithPronunciationCheckBox, Bottom, 10);
		UICOMP_CON_W(guessPronunciationCheckBox, &g_GuessPronunciationCheckBox, Horizontal, None, 25, checkBoxFont, questionTypeSection2);
		UIMARG_CON(guessPronunciationCheckBox, Bottom, 10);

		UICOMP_CON(guessPronunciationOptionSection, Horizontal, None, 25, questionTypeSection3);
		UIMARG_CON(guessPronunciationOptionSection, Bottom, 10);
		UICOMP_DOW_W(guessPronunciationWithMeaningCheckBox, &g_GuessPronunciationWithMeaningCheckBox, Vertical, None, 50, checkBoxFont, guessPronunciationOptionSection);
		UIMARG_CON(guessPronunciationWithMeaningCheckBox, Right, 1);
		UICOMP_DOW_W(groupGuessingMeaningAndPronunciationCheckBox, &g_GroupGuessingMeaningAndPronunciationCheckBox, Vertical, None, 50, checkBoxFont, guessPronunciationOptionSection);

		UICOMP_CON_W(otherOptionStatic, &g_OtherOptionStatic, Horizontal, None, 23, buttonFont, section2);
		UIMARG_CON(otherOptionStatic, Bottom, 5);
		UICOMP_CON_W(excludeDuplicatedAnswerCheckBox, &g_ExcludeDuplicatedAnswerCheckBox, Horizontal, None, 15, checkBoxFont, section2);

		UICOMP_WIN(buttonSection, None, section1);
		UIMARG_CON(buttonSection, Top, 10);

		UICOMP_DOW_W(mainButon, &g_MainButton, Vertical, None, 33, buttonFont, buttonSection);
		UIMARG_CON(mainButon, Right, 5);
		UICOMP_DOW_W(startButon, &g_StartButton, Vertical, None, 67, buttonFont, buttonSection);
		UIMARG_CON(startButon, Left, 5);
		return 0;
	}

	case AM_ACTIVATE:
		SetSceneTitle(handle, _T("단어 암기하기"));
		return 0;

	case WM_DESTROY:
		g_VocabularyPath = NULL;
		return 0;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case 0: {
			const LPCTSTR path = ShowOpenFileDialog(handle);
			if (path) {
				SetWindowText(g_SelectVocabularyButton, _T("단어장 선택됨"));
				g_VocabularyPath = path;
			}
			break;
		}

		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
			UpdateQuestionTypeCheckBoxVisibility();
			break;

		case 9:
			DestroyWindow(ChangeScene(MainWindow, CreateScene(MainWindow, MainSceneProc)));
			break;
		}
		return 0;

	default:
		return DefSubclassProc(handle, message, wParam, lParam);
	}
}

void UpdateQuestionTypeCheckBoxVisibility() {
	ShowWindow(g_GuessMeaningWithPronunciationCheckBox,
		SendMessage(g_GuessMeaningCheckBox, BM_GETCHECK, 0, 0) == BST_CHECKED ? SW_SHOW : SW_HIDE);
	ShowWindow(g_GuessWordWithPronunciationCheckBox,
		SendMessage(g_GuessWordCheckBox, BM_GETCHECK, 0, 0) == BST_CHECKED &&
		SendMessage(g_GroupGuessingMeaningAndPronunciationCheckBox, BM_GETCHECK, 0, 0) == BST_UNCHECKED ? SW_SHOW : SW_HIDE);
	ShowWindow(g_GuessPronunciationWithMeaningCheckBox,
		SendMessage(g_GuessPronunciationCheckBox, BM_GETCHECK, 0, 0) == BST_CHECKED ? SW_SHOW : SW_HIDE);
	ShowWindow(g_GroupGuessingMeaningAndPronunciationCheckBox,
		SendMessage(g_GuessWordCheckBox, BM_GETCHECK, 0, 0) == BST_CHECKED &&
		SendMessage(g_GuessPronunciationCheckBox, BM_GETCHECK, 0, 0) == BST_CHECKED &&
		SendMessage(g_GuessWordWithPronunciationCheckBox, BM_GETCHECK, 0, 0) == BST_UNCHECKED ? SW_SHOW : SW_HIDE);
}