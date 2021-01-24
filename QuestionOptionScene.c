#include "Window.h"

#include "Application.h"
#include "UIEngine.h"
#include "Word.h"

static HWND g_SelectVocabularyStatic, g_SelectVocabularyButton;
static LPCTSTR g_VocabularyPath;

static HWND g_QuestionTypeStatic;
static HWND g_GuessMeaningCheckBox, g_GuessMeaningWithoutPronunciationRadioButton, g_GuessMeaningWithPronunciationRadioButton, g_GuessMeaningAndPronunciationRadioButton;
static HWND g_GuessWordCheckBox, g_GuessWordWithoutPronunciationRadioButton, g_GuessWordWithPronunciationRadioButton, g_GuessWordAndPronunciationRadioButton;
static HWND g_GuessPronunciationCheckBox;
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
		g_GuessMeaningWithoutPronunciationRadioButton = CreateRadioButton(_T("발음 표시하지 않기"), WS_GROUP, handle, 2);
		g_GuessMeaningWithPronunciationRadioButton = CreateRadioButton(_T("발음 표시하기"), 0, handle, 3);
		g_GuessMeaningAndPronunciationRadioButton = CreateRadioButton(_T("발음도 맞히기"), 0, handle, 4);
		g_GuessWordCheckBox = CreateCheckBox(_T("뜻 보고 단어 맞히기"), WS_VISIBLE, handle, 5);
		g_GuessWordWithoutPronunciationRadioButton = CreateRadioButton(_T("발음 표시하지 않기"), WS_GROUP, handle, 6);
		g_GuessWordWithPronunciationRadioButton = CreateRadioButton(_T("발음 표시하기"), 0, handle, 7);
		g_GuessWordAndPronunciationRadioButton = CreateRadioButton(_T("발음도 맞히기"), 0, handle, 8);
		g_GuessPronunciationCheckBox = CreateCheckBox(_T("단어와 뜻 보고 발음 맞히기"), WS_VISIBLE, handle, 9);

		g_OtherOptionStatic = CreateStatic(_T("기타 옵션"), WS_VISIBLE | SS_LEFT, handle, -1);
		g_ExcludeDuplicatedAnswerCheckBox = CreateCheckBox(_T("중복된 문제 제외하기"), WS_VISIBLE, handle, 10);

		g_MainButton = CreateButton(_T("이전으로"), WS_VISIBLE, handle, 11);
		g_StartButton = CreateButton(_T("시작하기"), WS_VISIBLE, handle, 12);

		SendMessage(g_GuessMeaningCheckBox, BM_SETCHECK, Setting.GuessMeaning, 0);
		SendMessage(g_GuessMeaningWithoutPronunciationRadioButton, BM_SETCHECK, !Setting.GuessMeaningWithPronunciation || Setting.GuessMeaningWithPronunciation > 2, 0);
		SendMessage(g_GuessMeaningWithPronunciationRadioButton, BM_SETCHECK, !(Setting.GuessMeaningWithPronunciation - 1), 0);
		SendMessage(g_GuessMeaningAndPronunciationRadioButton, BM_SETCHECK, !(Setting.GuessMeaningWithPronunciation - 2), 0);
		SendMessage(g_GuessWordCheckBox, BM_SETCHECK, Setting.GuessWord, 0);
		SendMessage(g_GuessWordWithoutPronunciationRadioButton, BM_SETCHECK, !Setting.GuessWordWithPronunciation || Setting.GuessWordWithPronunciation > 2, 0);
		SendMessage(g_GuessWordWithPronunciationRadioButton, BM_SETCHECK, !(Setting.GuessWordWithPronunciation - 1), 0);
		SendMessage(g_GuessWordAndPronunciationRadioButton, BM_SETCHECK, !(Setting.GuessWordWithPronunciation - 2), 0);
		SendMessage(g_GuessPronunciationCheckBox, BM_SETCHECK, Setting.GuessPronunciation, 0);
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
		UICOMP_CON(questionTypeOptionSection1, Vertical, None, 155, questionTypeSection3);
		UICOMP_CON(questionTypeOptionSection2, Vertical, None, 121, questionTypeSection3);
		UICOMP_WIN(questionTypeOptionSection3, None, questionTypeSection3);

		UICOMP_CON_W(guessMeaningCheckBox, &g_GuessMeaningCheckBox, Horizontal, None, 25, checkBoxFont, questionTypeSection2);
		UIMARG_CON(guessMeaningCheckBox, Bottom, 10);
		UICOMP_CON_W(guessMeaningWithoutPronunciationRadioButton, &g_GuessMeaningWithoutPronunciationRadioButton, Horizontal, None, 25, checkBoxFont, questionTypeOptionSection1);
		UIMARG_CON(guessMeaningWithoutPronunciationRadioButton, Bottom, 10);
		UICOMP_CON_W(guessMeaningWithPronunciationRadioButton, &g_GuessMeaningWithPronunciationRadioButton, Horizontal, None, 25, checkBoxFont, questionTypeOptionSection2);
		UIMARG_CON(guessMeaningWithPronunciationRadioButton, Bottom, 10);
		UICOMP_CON_W(guessMeaningAndPronunciationRadioButton, &g_GuessMeaningAndPronunciationRadioButton, Horizontal, None, 25, checkBoxFont, questionTypeOptionSection3);
		UIMARG_CON(guessMeaningAndPronunciationRadioButton, Bottom, 10);
		UICOMP_CON_W(guessWordCheckBox, &g_GuessWordCheckBox, Horizontal, None, 25, checkBoxFont, questionTypeSection2);
		UIMARG_CON(guessWordCheckBox, Bottom, 10);
		UICOMP_CON_W(guessWordWithoutPronunciationRadioButton, &g_GuessWordWithoutPronunciationRadioButton, Horizontal, None, 25, checkBoxFont, questionTypeOptionSection1);
		UIMARG_CON(guessWordWithoutPronunciationRadioButton, Bottom, 10);
		UICOMP_CON_W(guessWordWithPronunciationRadioButton, &g_GuessWordWithPronunciationRadioButton, Horizontal, None, 25, checkBoxFont, questionTypeOptionSection2);
		UIMARG_CON(guessWordWithPronunciationRadioButton, Bottom, 10);
		UICOMP_CON_W(guessWordAndPronunciationRadioButton, &g_GuessWordAndPronunciationRadioButton, Horizontal, None, 25, checkBoxFont, questionTypeOptionSection3);
		UIMARG_CON(guessWordAndPronunciationRadioButton, Bottom, 10);
		UICOMP_CON_W(guessPronunciationCheckBox, &g_GuessPronunciationCheckBox, Horizontal, None, 30, checkBoxFont, questionTypeSection2);
		UIMARG_CON(guessPronunciationCheckBox, Bottom, 15);

		UICOMP_CON_W(otherOptionStatic, &g_OtherOptionStatic, Horizontal, None, 23, buttonFont, section2);
		UIMARG_CON(otherOptionStatic, Bottom, 5);
		UICOMP_CON_W(excludeDuplicatedAnswerCheckBox, &g_ExcludeDuplicatedAnswerCheckBox, Horizontal, None, 15, checkBoxFont, section2);

		UICOMP_WIN(buttonSection, None, section1);
		UIMARG_CON(buttonSection, Top, 10);

		UICOMP_DOW_W(mainButon, &g_MainButton, Vertical, None, 33.35f, buttonFont, buttonSection);
		UIMARG_CON(mainButon, Right, 5);
		UICOMP_DOW_W(startButon, &g_StartButton, Vertical, None, 66.65f, buttonFont, buttonSection);
		UIMARG_CON(startButon, Left, 5);
		return 0;
	}

	case AM_ACTIVATE:
		SetSceneTitle(handle, _T("단어 암기하기"));
		return 0;

	case AM_DESTROY:
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
		case 5:
			UpdateQuestionTypeCheckBoxVisibility();
			break;

		case 11:
			DestroyWindow(ChangeScene(MainWindow, CreateScene(MainWindow, MainSceneProc)));
			break;

		case 12:
			// TODO
			DestroyWindow(ChangeScene(MainWindow, CreateScene(MainWindow, QuestionSceneProc)));
			break;
		}
		return 0;

	default:
		return DefSubclassProc(handle, message, wParam, lParam);
	}
}

void UpdateQuestionTypeCheckBoxVisibility() {
	const int guessMeaningVisibility = SendMessage(g_GuessMeaningCheckBox, BM_GETCHECK, 0, 0) == BST_CHECKED ? SW_SHOW : SW_HIDE;
	ShowWindow(g_GuessMeaningWithoutPronunciationRadioButton, guessMeaningVisibility);
	ShowWindow(g_GuessMeaningWithPronunciationRadioButton, guessMeaningVisibility);
	ShowWindow(g_GuessMeaningAndPronunciationRadioButton, guessMeaningVisibility);

	const int guessWordVisibility = SendMessage(g_GuessWordCheckBox, BM_GETCHECK, 0, 0) == BST_CHECKED ? SW_SHOW : SW_HIDE;
	ShowWindow(g_GuessWordWithoutPronunciationRadioButton, guessWordVisibility);
	ShowWindow(g_GuessWordWithPronunciationRadioButton, guessWordVisibility);
	ShowWindow(g_GuessWordAndPronunciationRadioButton, guessWordVisibility);
}