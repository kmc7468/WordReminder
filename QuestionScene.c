#include "Window.h"

#include "UIEngine.h"
#include "Word.h"

static HWND g_DescriptionStatic;
static HWND g_QuestionStatic, g_HintStatic;
static Question g_Question;
static int g_AnswerState = 0;

static HWND g_Selectors[5];
static HWND g_PronunciationSelectors[5];
static int g_CheckedSelector = -1, g_CheckedPronunciationSelector = -1;

static void UpdateQuestion(UIEngine* uiEngine, bool generateQuestion);

static HWND g_StopButton;

LRESULT CALLBACK QuestionSceneProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR dummy0, DWORD_PTR dummy1) {
	EVENT {
	case AM_CREATE:
		g_DescriptionStatic = CreateStatic(NULL, WS_VISIBLE | SS_CENTER, handle, -1);
		g_QuestionStatic = CreateStatic(NULL, WS_VISIBLE | SS_CENTER, handle, -1);
		g_HintStatic = CreateStatic(NULL, WS_VISIBLE | SS_CENTER, handle, -1);

		for (int i = 0; i < 5; ++i) {
			g_Selectors[i] = CreateButton(NULL, WS_VISIBLE | BS_MULTILINE | BS_NOTIFY, handle, i);
			g_PronunciationSelectors[i] = CreateButton(NULL, BS_MULTILINE | BS_NOTIFY, handle, i + 5);
		}

		g_StopButton = CreateButton(_T("그만 외우기"), WS_VISIBLE, handle, 10);
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
		SetSceneTitle(handle, _T("단어 암기하기"));
		return 0;

	case AM_DESTROY:
		g_AnswerState = 0;

		g_CheckedSelector = -1;
		g_CheckedPronunciationSelector = -1;
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
		if (LOWORD(wParam) < 10) {
			if (HIWORD(wParam) != BN_CLICKED) return 0;

			const bool isSelector = LOWORD(wParam) < 5;
			for (int i = 0; i < 5; ++i) {
				SendMessage((isSelector ? g_Selectors : g_PronunciationSelectors)[i], BM_SETSTATE, i == LOWORD(wParam) % 5, 0);
			}

			if (isSelector) {
				g_CheckedSelector = LOWORD(wParam) % 5;
				if (g_Question.Type->Option != 2) {
					g_CheckedPronunciationSelector = g_Question.PronunciationAnswer;
				}
			} else {
				g_CheckedPronunciationSelector = LOWORD(wParam) % 5;
			}

			if (g_CheckedSelector != -1 && g_CheckedPronunciationSelector != -1) {
				if (g_CheckedSelector == g_Question.Answer && g_CheckedPronunciationSelector == g_Question.PronunciationAnswer) {
					if (g_AnswerState != 0) {
						g_Question.Meanings[g_CheckedSelector]->IsWrong = true;
					}

					SendMessage(g_Selectors[g_CheckedSelector], BM_SETSTATE, FALSE, 0);
					SendMessage(g_PronunciationSelectors[g_CheckedPronunciationSelector], BM_SETSTATE, FALSE, 0);
					g_CheckedSelector = -1;
					g_CheckedPronunciationSelector = -1;

					g_AnswerState = 0;
					UpdateQuestion(GetUIEngine(handle), true);
				} else {
					int localAnswerState = 0;
					if (g_CheckedSelector != g_Question.Answer) {
						g_Question.Meanings[g_CheckedSelector]->IsWrong = true;

						SendMessage(g_Selectors[g_CheckedSelector], BM_SETSTATE, FALSE, 0);
						EnableWindow(g_Selectors[g_CheckedSelector], FALSE);
						g_CheckedSelector = -1;

						g_AnswerState |= 1;
						localAnswerState |= 1;
					}
					if (g_CheckedPronunciationSelector != g_Question.PronunciationAnswer && g_Question.Type->Option == 2) {
						for (int i = 0; i < 5; ++i) {
							if (g_Question.Pronunciations[i] == g_CheckedPronunciationSelector) {
								g_Question.Meanings[i]->IsWrong = true;
								break;
							}
						}

						SendMessage(g_PronunciationSelectors[g_CheckedPronunciationSelector], BM_SETSTATE, FALSE, 0);
						EnableWindow(g_PronunciationSelectors[g_CheckedPronunciationSelector], FALSE);
						g_CheckedPronunciationSelector = -1;

						g_AnswerState |= 2;
						localAnswerState |= 2;
					}

					static const LPCTSTR wrongDescription[] = {
						_T("단어를 잘못 골랐습니다"), _T("뜻을 잘못 골랐습니다"), _T("발음을 잘못 골랐습니다"),
						_T("단어와 발음을 잘못 골랐습니다"), _T("뜻과 발음을 잘못 골랐습니다"),
					};
					static const int wrongDescriptionTable[3][4] = {
						{ -1, 1, 2, 4 },
						{ -1, 0, 2, 3 },
						{ -1, 2, -1, -1 },
					};
					SetWindowText(g_DescriptionStatic, wrongDescription[wrongDescriptionTable[g_Question.Type->Type][localAnswerState]]);
				}
			}
		} else switch (LOWORD(wParam)) {
		case 10: {
			const HWND statisticScene = CreateScene(MainWindow, StatisticSceneProc);
			SendMessage(statisticScene, AM_DATA, DT_QUESTIONOPTION, (LPARAM)g_Question.Option);

			DestroyWindow(ChangeScene(MainWindow, statisticScene));
			break;
		}
		}
		return 0;

	case WM_NOTIFY: {
		const LPNMHDR header = (LPNMHDR)lParam;
		switch (header->code) {
		case BCN_HOTITEMCHANGE: {
			const LPNMBCHOTITEM hotItem = (LPNMBCHOTITEM)lParam;
			const bool entered = hotItem->dwFlags & HICF_ENTERING;
			if (entered && g_Question.Type->Option == 2) {
				const UIEngine* uiEngine = GetUIEngine(handle);

				for (int i = 0; i < 5; ++i) {
					TCHAR enabledName[] = _T("EnabledSelectorN"), disabledName[] = _T("DisabledSelectorN"),
						enabledPronunciationName[] = _T("EnabledPronunciationSelectorN"), disabledPronunciationName[] = _T("DisabledPronunciationSelectorN");
					enabledName[ARRAYSIZE(enabledName) - 2] = i + 1 + '0';
					disabledName[ARRAYSIZE(disabledName) - 2] = i + 1 + '0';
					enabledPronunciationName[ARRAYSIZE(enabledPronunciationName) - 2] = i + 1 + '0';
					disabledPronunciationName[ARRAYSIZE(disabledPronunciationName) - 2] = i + 1 + '0';

					UICOMP_FIND(enabledSelector, enabledName);
					UICOMP_FIND(disabledSelector, disabledName);
					UICOMP_FIND(enabledPronunciationSelector, enabledPronunciationName);
					UICOMP_FIND(disabledPronunciationSelector, disabledPronunciationName);

					if ((int)GetMenu(hotItem->hdr.hwndFrom) < 5) {
						enabledSelector->Window = g_Selectors + i;
						disabledSelector->Window = NULL;
						enabledPronunciationSelector->Window = NULL;
						disabledPronunciationSelector->Window = g_PronunciationSelectors + i;
					} else {
						enabledSelector->Window = NULL;
						disabledSelector->Window = g_Selectors + i;
						enabledPronunciationSelector->Window = g_PronunciationSelectors + i;
						disabledPronunciationSelector->Window = NULL;
					}
				}

				UpdateUIEngine(uiEngine);
			}
			break;
		}
		}
		return 0;
	}

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
		SetWindowText(g_DescriptionStatic, _T("다음 단어의 뜻은?"));
		SetWindowText(g_QuestionStatic, GetWord(&g_Question.Option->Vocabulary, g_Question.Meanings[g_Question.Answer]->Word)->Word);
		if (g_Question.Type->Option == 1) {
			SetWindowText(g_HintStatic, g_Question.Meanings[g_Question.Answer]->Pronunciation);
		}
		break;

	case GuessWord:
		SetWindowText(g_DescriptionStatic, _T("다음 뜻을 가진 단어는?"));
		SetWindowText(g_QuestionStatic, g_Question.Meanings[g_Question.Answer]->Meaning);
		break;

	case GuessPronunciation:
		SetWindowText(g_DescriptionStatic, _T("다음 단어의 발음은?"));
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
			SetWindowText(g_PronunciationSelectors[g_Question.Pronunciations[i]], g_Question.Meanings[i]->Pronunciation);
		}

		EnableWindow(g_Selectors[i], TRUE);
		EnableWindow(g_PronunciationSelectors[i], TRUE);
	}
}