#include "Window.h"

#include "Application.h"
#include "UIEngine.h"
#include "Word.h"

static HWND g_SelectVocabularyStatic, g_SelectVocabularyButton;
static LPCTSTR g_VocabularyPath;
static Vocabulary* g_Vocabulary;

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
		g_SelectVocabularyStatic = CreateStatic(_T("�ϱ��� �ܾ���"), WS_VISIBLE | SS_LEFT, handle, -1);
		g_SelectVocabularyButton = CreateButton(_T("�ܾ��� �����ϱ�"), WS_VISIBLE, handle, 0);

		g_QuestionTypeStatic = CreateStatic(_T("���� ����"), WS_VISIBLE | SS_LEFT, handle, -1);
		g_GuessMeaningCheckBox = CreateCheckBox(_T("�ܾ� ���� �� ������"), WS_VISIBLE, handle, 1);
		g_GuessMeaningWithoutPronunciationRadioButton = CreateRadioButton(_T("���� ǥ������ �ʱ�"), WS_GROUP, handle, 2);
		g_GuessMeaningWithPronunciationRadioButton = CreateRadioButton(_T("���� ǥ���ϱ�"), 0, handle, 3);
		g_GuessMeaningAndPronunciationRadioButton = CreateRadioButton(_T("������ ������"), 0, handle, 4);
		g_GuessWordCheckBox = CreateCheckBox(_T("�� ���� �ܾ� ������"), WS_VISIBLE, handle, 5);
		g_GuessWordWithoutPronunciationRadioButton = CreateRadioButton(_T("���� ǥ������ �ʱ�"), WS_GROUP, handle, 6);
		g_GuessWordWithPronunciationRadioButton = CreateRadioButton(_T("���� ǥ���ϱ�"), 0, handle, 7);
		g_GuessWordAndPronunciationRadioButton = CreateRadioButton(_T("������ ������"), 0, handle, 8);
		g_GuessPronunciationCheckBox = CreateCheckBox(_T("�ܾ�� �� ���� ���� ������"), WS_VISIBLE, handle, 9);

		g_OtherOptionStatic = CreateStatic(_T("��Ÿ �ɼ�"), WS_VISIBLE | SS_LEFT, handle, -1);
		g_ExcludeDuplicatedAnswerCheckBox = CreateCheckBox(_T("�ߺ��� ���� �����ϱ�"), WS_VISIBLE, handle, 10);

		g_MainButton = CreateButton(_T("��������"), WS_VISIBLE, handle, 11);
		g_StartButton = CreateButton(_T("�����ϱ�"), WS_VISIBLE, handle, 12);

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
		SetSceneTitle(handle, _T("�ܾ� �ϱ��ϱ�"));
		return 0;

	case AM_DESTROY:
		g_VocabularyPath = NULL;

		if (g_Vocabulary) {
			DestroyVocabulary(g_Vocabulary, true);
			free(g_Vocabulary);
			g_Vocabulary = NULL;
		}
		return 0;

	case AM_DATA:
		switch (wParam) {
		case DT_VOCABULARY:
			g_Vocabulary = (Vocabulary*)lParam;
			for (int i = 0; i < g_Vocabulary->Words.Count; ++i) {
				Word* const word = GetWord(g_Vocabulary, i);
				for (int j = 0; j < word->Meanings.Count; ++j) {
					GetMeaning(word, j)->IsWrong = false;
				}
			}

			SetWindowText(g_SelectVocabularyButton, _T("�ܾ��� ���õ�"));
			SetWindowText(g_MainButton, _T("��������"));
			break;
		}
		return 0;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case 0: {
			const LPCTSTR path = ShowOpenFileDialog(handle);
			if (path) {
				SetWindowText(g_SelectVocabularyButton, _T("�ܾ��� ���õ�"));
				g_VocabularyPath = path;

				if (g_Vocabulary) {
					DestroyVocabulary(g_Vocabulary, true);
					free(g_Vocabulary);
					g_Vocabulary = NULL;
				}
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

		case 12: {
			if (!g_VocabularyPath && !g_Vocabulary) {
				MessageBox(handle, _T("�ϱ��� �ܾ����� �����ؾ� �մϴ�."), _T("����"), MB_OK | MB_ICONERROR);
				break;
			}

			const bool guessMeaning = (bool)SendMessage(g_GuessMeaningCheckBox, BM_GETCHECK, 0, 0);
			const bool guessWord = (bool)SendMessage(g_GuessWordCheckBox, BM_GETCHECK, 0, 0);
			const bool guessPronunciation = (bool)SendMessage(g_GuessPronunciationCheckBox, BM_GETCHECK, 0, 0);
			if (!(guessMeaning || guessWord || guessPronunciation)) {
				MessageBox(handle, _T("���� ������ ��� �ϳ� �����ؾ� �մϴ�."), _T("����"), MB_OK | MB_ICONERROR);
				break;
			}

			QuestionOption* option = calloc(1, sizeof(QuestionOption));
			CreateQuestionOption(option);
			if (!g_Vocabulary && !LoadVocabulary(&option->Vocabulary, g_VocabularyPath)) {
				MessageBox(handle, _T("�ܾ����� �д� �� ������ �߻��߽��ϴ�. �ùٸ� �ܾ������� Ȯ���� ���ʽÿ�."), _T("���"), MB_OK | MB_ICONERROR);

				DestroyQuestionOption(option);
				free(option);
				break;
			} else if (g_Vocabulary) {
				option->Vocabulary = *g_Vocabulary;

				free(g_Vocabulary);
				g_Vocabulary = NULL;
			}

			if (guessMeaning) {
				QuestionType type = { 0 };
				CreateQuestionType(&type);

				type.Type = GuessMeaning;
				if ((bool)SendMessage(g_GuessMeaningWithPronunciationRadioButton, BM_GETCHECK, 0, 0)) {
					type.Option = 1;
				} else if ((bool)SendMessage(g_GuessMeaningAndPronunciationRadioButton, BM_GETCHECK, 0, 0)) {
					if (!IsUsableVocabulary(&option->Vocabulary, GuessMeaning, 2)) {
						MessageBox(handle, _T("�ٸ� �ܾ�� ���� ������ ��� ������ ���� �ܾ ��� 5�� �̻� �־�� ������ ������ �ɼ��� ����� �� �ֽ��ϴ�."), _T("����"), MB_OK | MB_ICONERROR);

						g_Vocabulary = malloc(sizeof(Vocabulary));
						*g_Vocabulary = option->Vocabulary;
						DestroyQuestionOption(option);
						DestroyQuestionType(&type);
						free(option);
						break;
					}

					type.Option = 2;
				}

				AddElement(&option->Types, &type);
			}
			if (guessWord) {
				QuestionType type = { 0 };
				CreateQuestionType(&type);

				type.Type = GuessWord;
				if ((bool)SendMessage(g_GuessWordWithPronunciationRadioButton, BM_GETCHECK, 0, 0)) {
					type.Option = 1;
				} else if ((bool)SendMessage(g_GuessWordAndPronunciationRadioButton, BM_GETCHECK, 0, 0)) {
					if (!IsUsableVocabulary(&option->Vocabulary, GuessMeaning, 2)) {
						MessageBox(handle, _T("�ٸ� �ܾ�� ���� ������ ��� ������ ���� �ܾ ��� 5�� �̻� �־�� ������ ������ �ɼ��� ����� �� �ֽ��ϴ�."), _T("����"), MB_OK | MB_ICONERROR);

						g_Vocabulary = malloc(sizeof(Vocabulary));
						*g_Vocabulary = option->Vocabulary;
						DestroyQuestionOption(option);
						DestroyQuestionType(&type);
						free(option);
						break;
					}

					type.Option = 2;
				}

				AddElement(&option->Types, &type);
			}
			if (guessPronunciation) {
				if (!IsUsableVocabulary(&option->Vocabulary, GuessPronunciation, 0)) {
					MessageBox(handle, _T("�ٸ� �ܾ�� ���� ������ ������ ���� �ܾ ��� 5�� �̻� �־�� �ܾ�� �� ���� �ܾ� ���߱� ������ ����� �� �ֽ��ϴ�."), _T("����"), MB_OK | MB_ICONERROR);

					g_Vocabulary = malloc(sizeof(Vocabulary));
					*g_Vocabulary = option->Vocabulary;
					DestroyQuestionOption(option);
					free(option);
					break;
				}

				QuestionType type = { 0 };
				CreateQuestionType(&type);

				type.Type = GuessPronunciation;

				AddElement(&option->Types, &type);
			}

			option->ExcludeDuplicatedAnswer = (bool)SendMessage(g_ExcludeDuplicatedAnswerCheckBox, BM_GETCHECK, 0, 0);

			Setting.GuessMeaning = guessMeaning;
			Setting.GuessMeaningWithPronunciation = ((bool)SendMessage(g_GuessMeaningWithPronunciationRadioButton, BM_GETCHECK, 0, 0) ? 1
				: ((bool)SendMessage(g_GuessMeaningAndPronunciationRadioButton, BM_GETCHECK, 0, 0) ? 2 : 0));
			Setting.GuessWord = guessWord;
			Setting.GuessWordWithPronunciation = ((bool)SendMessage(g_GuessWordWithPronunciationRadioButton, BM_GETCHECK, 0, 0) ? 1
				: ((bool)SendMessage(g_GuessWordAndPronunciationRadioButton, BM_GETCHECK, 0, 0) ? 2 : 0));
			Setting.GuessPronunciation = guessPronunciation;

			Setting.ExcludeDuplicatedAnswer = option->ExcludeDuplicatedAnswer;

			const HWND questionScene = CreateScene(MainWindow, QuestionSceneProc);
			SendMessage(questionScene, AM_DATA, DT_QUESTIONOPTION, (LPARAM)option);

			DestroyWindow(ChangeScene(MainWindow, questionScene));
			break;
		}
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