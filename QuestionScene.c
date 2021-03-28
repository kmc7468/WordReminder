#include "Window.h"

#include "UIEngine.h"
#include "Word.h"

static HWND g_DescriptionStatic;
static HWND g_QuestionStatic, g_HintStatic;
static Question g_Question;

static HWND g_Selectors[5];
static HWND g_PronunciationSelectors[5];

static void UpdateQuestion(UIEngine* uiEngine, bool generateQuestion);

static HWND g_StopButton;

LRESULT CALLBACK QuestionSceneProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR dummy0, DWORD_PTR dummy1) {
	EVENT {
	case AM_CREATE:
		g_DescriptionStatic = CreateStatic(NULL, WS_VISIBLE | SS_CENTER, handle, -1);
		g_QuestionStatic = CreateStatic(NULL, WS_VISIBLE | SS_CENTER, handle, -1);
		g_HintStatic = CreateStatic(NULL, WS_VISIBLE | SS_CENTER, handle, -1);

		for (int i = 0; i < 5; ++i) {
			g_Selectors[i] = CreateButton(NULL, WS_VISIBLE | BS_MULTILINE, handle, i);
			g_PronunciationSelectors[i] = CreateButton(NULL, BS_MULTILINE, handle, i + 5);
		}

		g_StopButton = CreateButton(_T("�׸� �ܿ��"), WS_VISIBLE, handle, 10);
		return 0;

	case AM_CREATEUI: {
		UIEngine* const uiEngine = (UIEngine*)lParam;

		UIFONT_CON(buttonFont, 18, true);
		UIFONT_FCN(questionFont, 40, true);
		UIFONT_FCN(hintFont, 22, false);
		UIFONT_FCN(selectorFont, 18, false);

		UICOMP_DOH(section1, Horizontal, None, 100, &uiEngine->RootComponent);
		UIMARG_CON_AS(section1, 10);

		UICOMP_WIN(section2, None, section1);
		UIMARG_CON(section2, Bottom, -60);

		UICOMP_DOC(questionSection, Horizontal, None, section2);
		UICOMP_CON_W(descriptionStatic, &g_DescriptionStatic, Horizontal, None, 23, buttonFont, questionSection);
		UIMARG_CON(descriptionStatic, Bottom, 5);
		UICOMP_FCN_W(questionStatic, &g_QuestionStatic, Horizontal, None, 40, questionFont, questionSection);
		UICOMP_CON(hintStaticMargin1, Horizontal, None, 5, questionSection);
		UICOMP_FCN_W(hintStatic, &g_HintStatic, Horizontal, None, 22, hintFont, questionSection);
		UICOMP_CON(hintStaticMargin2, Horizontal, None, 10, questionSection);

		UICOMP_WIN(selectorSection1, Center, section2);
		UICOMP_DOW(selectorSection2, Vertical, Center, 50, selectorSection1);

		for (int i = 0; i < 5; ++i) {
			TCHAR defaultName[] = _T("SelectorN"), enabledName[] = _T("EnabledSelectorN"), disabledName[] = _T("DisabledSelectorN"),
				enabledPronunciationName[] = _T("EnabledPronunciationSelectorN"), disabledPronunciationName[] = _T("DisabledPronunciationSelectorN");
			defaultName[ARRAYSIZE(defaultName) - 2] = i + 1 + '0';
			enabledName[ARRAYSIZE(enabledName) - 2] = i + 1 + '0';
			disabledName[ARRAYSIZE(disabledName) - 2] = i + 1 + '0';
			enabledPronunciationName[ARRAYSIZE(enabledPronunciationName) - 2] = i + 1 + '0';
			disabledPronunciationName[ARRAYSIZE(disabledPronunciationName) - 2] = i + 1 + '0';

			UICOMP_DOC(selectorSection, Horizontal, None, selectorSection2);

			UICOMP_DOC_N(selector, defaultName, Horizontal, None, selectorFont, selectorSection);
			selector->Window = &g_Selectors[i];
			UICOMP_DOC(selectorButton, Horizontal, None, selector);
			selectorButton->IsOverridable = true;
			UICOMP_FCN(selectorText, Horizontal, None, 36, selectorButton);
			UICOMP_CON(selectorBody, Horizontal, None, 14, selectorButton);

			if (i < 4) {
				UICOMP_CON(selectorMargin, Horizontal, None, 10, selectorSection);
			}

			UICOMP_WIN(enabledSelectorSection, None, selector);
			enabledSelectorSection->IsOverridable = true;
			UICOMP_DOW_N(enabledSelector, enabledName, Vertical, None, 80, selectorFont, enabledSelectorSection);
			UIMARG_CON(enabledSelector, Right, 5);
			UICOMP_DOW_N(disabledPronunciationSelector, disabledPronunciationName, Vertical, None, 20, selectorFont, enabledSelectorSection);
			UIMARG_CON(disabledPronunciationSelector, Left, 5);

			UICOMP_WIN(disabledSelectorSection, None, selector);
			UICOMP_DOW_N(disabledSelector, disabledName, Vertical, None, 20, selectorFont, disabledSelectorSection);
			UIMARG_CON(disabledSelector, Right, 5);
			UICOMP_DOW_N(enabledPronunciationSelector, enabledPronunciationName, Vertical, None, 80, selectorFont, disabledSelectorSection);
			UIMARG_CON(enabledPronunciationSelector, Left, 5);
		}

		UICOMP_WIN(buttonSection, None, section1);
		UIMARG_CON(buttonSection, Top, 10);

		UICOMP_DOW_N(buttonSection11, _T("ButtonSection1/1"), Vertical, None, 100, buttonFont, buttonSection);
		UICOMP_DOW_N(buttonSection12, _T("ButtonSection1/2"), Vertical, None, 50, buttonFont, buttonSection);
		UIMARG_CON(buttonSection12, Right, 5);
		UICOMP_DOW_N(buttonSection22, _T("ButtonSection2/2"), Vertical, None, 50, buttonFont, buttonSection);
		UIMARG_CON(buttonSection22, Left, 5);

		buttonSection11->Window = &g_StopButton;
		return 0;
	}

	case AM_ACTIVATE:
		SetSceneTitle(handle, _T("�ܾ� �ϱ��ϱ�"));
		return 0;

	case AM_DATA:
		switch (wParam) {
		case DT_QUESTIONOPTION:
			g_Question.Option = (QuestionOption*)lParam;
			g_Question.Option->NumberOfSelectors = 5;

			UpdateQuestion(GetUIEngine(handle), true);
			break;
		}
		return 0;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case 10:
			// TODO
			break;
		}
		return 0;

	default:
		return DefSubclassProc(handle, message, wParam, lParam);
	}
}

void UpdateQuestion(UIEngine* uiEngine, bool generateQuestion) {
	const int oldTypeOption = g_Question.Type ? g_Question.Type->Option : 0;

	if (generateQuestion) {
		GenerateQuestion(&g_Question, NULL);
	}

	const bool needTwoButtons = oldTypeOption <= 1 && g_Question.Type->Option == 2,
		needOneButton = oldTypeOption == 2 && g_Question.Type->Option <= 1;
	if (needTwoButtons || needOneButton) {
		for (int i = 0; i < 5; ++i) {
			TCHAR defaultName[] = _T("SelectorN"), enabledName[] = _T("EnabledSelectorN"), disabledName[] = _T("DisabledSelectorN"),
				enabledPronunciationName[] = _T("EnabledPronunciationSelectorN"), disabledPronunciationName[] = _T("DisabledPronunciationSelectorN");
			defaultName[ARRAYSIZE(defaultName) - 2] = i + 1 + '0';
			enabledName[ARRAYSIZE(enabledName) - 2] = i + 1 + '0';
			disabledName[ARRAYSIZE(disabledName) - 2] = i + 1 + '0';
			enabledPronunciationName[ARRAYSIZE(enabledPronunciationName) - 2] = i + 1 + '0';
			disabledPronunciationName[ARRAYSIZE(disabledPronunciationName) - 2] = i + 1 + '0';

			if (needTwoButtons) {
				UICOMP_FIND(selector, defaultName);
				selector->Window = NULL;

				UICOMP_FIND(enabledSelector, enabledName);
				enabledSelector->Window = g_Selectors + i;

				UICOMP_FIND(disabledPronunciationSelector, disabledPronunciationName);
				disabledPronunciationSelector->Window = g_PronunciationSelectors + i;
				ShowWindow(g_PronunciationSelectors[i], SW_SHOW);
			} else {
				UICOMP_FIND(selector, defaultName);
				selector->Window = g_Selectors + i;

				UICOMP_FIND(enabledSelector, enabledName);
				UICOMP_FIND(disabledSelector, disabledName);
				enabledSelector->Window = NULL;
				disabledSelector->Window = NULL;

				UICOMP_FIND(enabledPronunciationSelector, enabledPronunciationName);
				UICOMP_FIND(disabledPronunciationSelector, disabledPronunciationName);
				enabledPronunciationSelector->Window = NULL;
				disabledPronunciationSelector->Window = NULL;
				ShowWindow(g_PronunciationSelectors[i], SW_HIDE);
			}
		}

		UpdateUIEngine(uiEngine);
	}

	switch (g_Question.Type->Type) {
	case GuessMeaning:
		SetWindowText(g_DescriptionStatic, _T("���� �ܾ��� ����?"));
		SetWindowText(g_QuestionStatic, GetWord(&g_Question.Option->Vocabulary, g_Question.Meanings[g_Question.Answer]->Word)->Word);
		if (g_Question.Type->Option == 1) {
			SetWindowText(g_HintStatic, g_Question.Meanings[g_Question.Answer]->Pronunciation);
		}
		break;

	case GuessWord:
		SetWindowText(g_DescriptionStatic, _T("���� ���� ���� �ܾ��?"));
		SetWindowText(g_QuestionStatic, g_Question.Meanings[g_Question.Answer]->Meaning);
		break;

	case GuessPronunciation:
		SetWindowText(g_DescriptionStatic, _T("���� �ܾ��� ������?"));
		SetWindowText(g_QuestionStatic, GetWord(&g_Question.Option->Vocabulary, g_Question.Meanings[g_Question.Answer]->Word)->Word);
		SetWindowText(g_HintStatic, g_Question.Meanings[g_Question.Answer]->Meaning);
		break;
	}

	for (int i = 0; i < 5; ++i) {
		if (g_Question.Type->Type == GuessMeaning) {
			SetWindowText(g_Selectors[i], g_Question.Meanings[i]->Meaning);
		} else if (g_Question.Type->Type == GuessPronunciation) {
			SetWindowText(g_Selectors[i], g_Question.Meanings[i]->Pronunciation);
		} else {
			const Word* const word = GetWord(&g_Question.Option->Vocabulary, g_Question.Meanings[i]->Word);
			if (g_Question.Type->Option != 1) {
				SetWindowText(g_Selectors[i], word->Word);
			} else {
				LPTSTR text = malloc(sizeof(TCHAR) * (_tcslen(word->Word) + _tcslen(g_Question.Meanings[i]->Pronunciation) + 4));
				_tcscpy(text, word->Word);
				_tcscat(text, _T("\n("));
				_tcscat(text, g_Question.Meanings[i]->Pronunciation);
				_tcscat(text, _T(")"));

				SetWindowText(g_Selectors[i], text);
				free(text);
			}
		}

		if (g_Question.Type->Option == 2) {
			SetWindowText(g_PronunciationSelectors[i], g_Question.Meanings[i]->Pronunciation);
		}
	}
}