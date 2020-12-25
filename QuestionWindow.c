#include "Window.h"

#include "Multiplay.h"
#include "Word.h"

#include <stdlib.h>
#include <time.h>

static void CreateChangeRoleButton(HWND handle, RECT windowSize);
static void ShowNextQuestion(HWND handle, bool generateQuestion);

static HFONT g_QuestionFont, g_WordOrMeaningFont, g_MeaningOrPronunciationFont, g_ButtonFont;
static HWND g_Buttons[5];
static HWND g_ChangeRoleButton, g_StopButton;

static QuestionOption* g_QuestionOption;
static Question g_Question;
static Vocabulary g_UnusedVocabulary[ARRAYSIZE(QuestionTypes)];
static bool g_IsWrong;
static OnlineMultiplay* g_Multiplay;

static bool g_ShouldEnableMainWindow = true;

LRESULT CALLBACK QuestionWindowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam) {
	EVENT {
	case WM_CREATE:
		g_QuestionFont = CreateGlobalFont(23, true);
		g_WordOrMeaningFont = CreateGlobalFont(40, true);
		g_MeaningOrPronunciationFont = CreateGlobalFont(28, false);
		g_ButtonFont = CreateGlobalFont(18, false);

		for (int i = 0; i < 5; ++i) {
			g_Buttons[i] = CreateAndShowChild(_T("button"), NULL, g_ButtonFont, BS_PUSHBUTTON | BS_MULTILINE,
				WIDTH / 2 - WIDTH / 4, 140 + ((HEIGHT / 10 + HEIGHT / 50) * i), WIDTH / 2, HEIGHT / 10, handle, i);
		}

		g_StopButton = CreateAndShowChild(_T("button"), _T("�׸� �ܿ��"), g_ButtonFont, BS_PUSHBUTTON,
			WIDTH - WIDTH / 4 + 10, 140 + ((HEIGHT / 10 + HEIGHT / 50) * 4), WIDTH / 4 - 37, HEIGHT / 10, handle, 6);

		g_Question.Answer = -1;
		return 0;

	case WM_DESTROY:
		DeleteObject(g_QuestionFont);
		DeleteObject(g_WordOrMeaningFont);
		DeleteObject(g_MeaningOrPronunciationFont);
		DeleteObject(g_ButtonFont);

		g_ChangeRoleButton = NULL;

		if (g_QuestionOption) {
			DestroyVocabulary(&g_QuestionOption->Vocabulary);
			free(g_QuestionOption);

			for (int i = 0; i < ARRAYSIZE(QuestionTypes); ++i) {
				DestroyVocabulary(g_UnusedVocabulary + i);
			}
		}
		g_IsWrong = false;
		if (g_Multiplay) {
			StopOnlineMultiplay(g_Multiplay);
			free(g_Multiplay);
			g_Multiplay = NULL;
		}

		if (g_ShouldEnableMainWindow) {
			EnableWindow(MainWindow, TRUE);
			SetWindowPos(MainWindow, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}
		g_ShouldEnableMainWindow = true;
		return 0;

	case WM_SIZE:
		DeleteObject(g_WordOrMeaningFont);
		DeleteObject(g_MeaningOrPronunciationFont);
		DeleteObject(g_ButtonFont);
		g_WordOrMeaningFont = CreateGlobalFont(GetAppropriateFontSize(WIDTH, HEIGHT, 40), true);
		g_MeaningOrPronunciationFont = CreateGlobalFont(GetAppropriateFontSize(WIDTH, HEIGHT, 28), false);
		g_ButtonFont = CreateGlobalFont(GetAppropriateFontSize(WIDTH, HEIGHT, 18), false);

		for (int i = 0; i < 5; ++i) {
			SetWindowPos(g_Buttons[i], HWND_TOP, WIDTH / 2 - WIDTH / 4, 75 + GetAppropriateFontSize(WIDTH, HEIGHT, 35) + GetAppropriateFontSize(WIDTH, HEIGHT, 30) + ((HEIGHT / 10 + HEIGHT / 50) * i),
				WIDTH / 2, HEIGHT / 10, 0);
			SendMessage(g_Buttons[i], WM_SETFONT, (WPARAM)g_ButtonFont, true);
		}

		SetWindowPos(g_ChangeRoleButton, HWND_TOP, WIDTH - WIDTH / 4 + 10, 75 + GetAppropriateFontSize(WIDTH, HEIGHT, 35) + GetAppropriateFontSize(WIDTH, HEIGHT, 30) + ((HEIGHT / 10 + HEIGHT / 50) * 3),
			WIDTH / 4 - 37, HEIGHT / 10, 0);
		SetWindowPos(g_StopButton, HWND_TOP, WIDTH - WIDTH / 4 + 10, 75 + GetAppropriateFontSize(WIDTH, HEIGHT, 35) + GetAppropriateFontSize(WIDTH, HEIGHT, 30) + ((HEIGHT / 10 + HEIGHT / 50) * 4),
			WIDTH / 4 - 37, HEIGHT / 10, 0);
		SendMessage(g_ChangeRoleButton, WM_SETFONT, (WPARAM)g_ButtonFont, true);
		SendMessage(g_StopButton, WM_SETFONT, (WPARAM)g_ButtonFont, true);
		return 0;

	case WM_PAINT: {
		BEGINPAINT;
		SetTextAlign(dc, TA_CENTER);

		if (g_Multiplay) {
			switch (g_Multiplay->Status) {
			case OpeningServer:
				DrawTextUsingFont(dc, g_QuestionFont, WIDTH / 2, HEIGHT / 2 - 30, STRING("������ ���� ��..."));
				break;
			case WaitingForPlayer:
				DrawTextUsingFont(dc, g_QuestionFont, WIDTH / 2, HEIGHT / 2 - 30, STRING("������ ��ٸ��� ��..."));
				break;
			case PlayerJoining:
				DrawTextUsingFont(dc, g_QuestionFont, WIDTH / 2, HEIGHT / 2 - 30, STRING("������ �����ϴ� ��..."));
				break;
			case JoiningServer:
				DrawTextUsingFont(dc, g_QuestionFont, WIDTH / 2, HEIGHT / 2 - 30, STRING("������ �����ϴ� ��..."));
				break;

			case Connected:
			case SentAnswer:
				if (g_Multiplay->Option->Role == Examiner) {
					DrawTextUsingFont(dc, g_QuestionFont, WIDTH / 2, 10, STRING("����� �������Դϴ�."));
					if (g_Multiplay->Status == Connected) {
						DrawTextUsingFont(dc, g_WordOrMeaningFont, WIDTH / 2, 50, STRING("� �ܾ �������� �ұ��?"));
					} else {
						DrawTextUsingFont(dc, g_WordOrMeaningFont, WIDTH / 2, 50, STRING("�����ڸ� ��ٸ��� ��..."));
					}
				} else if (g_Question.Answer == -1) {
					DrawTextUsingFont(dc, g_QuestionFont, WIDTH / 2, 10, STRING("����� �������Դϴ�."));
					DrawTextUsingFont(dc, g_WordOrMeaningFont, WIDTH / 2, 50, STRING("�����ڸ� ��ٸ��� ��..."));
				} else goto Default;
				break;
			}
		} else {
			const Word* answer;
		Default:
			answer = g_Question.Words[g_Question.Answer];
			switch (g_Question.Type) {
			case GuessMeaning:
				DrawTextUsingFont(dc, g_QuestionFont, WIDTH / 2, 10, STRING("���� �ܾ��� ����?"));
				DrawTextUsingFont(dc, g_WordOrMeaningFont, WIDTH / 2, 50, answer->Word, (int)_tcslen(answer->Word));
				if (g_QuestionOption->GivePronunciation && answer->Pronunciation[0] != 0 && _tcscmp(answer->Word, answer->Pronunciation)) {
					DrawTextUsingFont(dc, g_MeaningOrPronunciationFont, WIDTH / 2, 55 + GetAppropriateFontSize(WIDTH, HEIGHT, 35),
						answer->Pronunciation, (int)_tcslen(answer->Pronunciation));
				}
				break;

			case GuessWord:
				DrawTextUsingFont(dc, g_QuestionFont, WIDTH / 2, 10, STRING("���� ���� ���� �ܾ��?"));
				DrawTextUsingFont(dc, g_WordOrMeaningFont, WIDTH / 2, 50, answer->Meaning, (int)_tcslen(answer->Meaning));
				break;

			case GuessPronunciation:
				DrawTextUsingFont(dc, g_QuestionFont, WIDTH / 2, 10, STRING("���� �ܾ��� ������?"));
				DrawTextUsingFont(dc, g_WordOrMeaningFont, WIDTH / 2, 50, answer->Word, (int)_tcslen(answer->Word));
				DrawTextUsingFont(dc, g_MeaningOrPronunciationFont, WIDTH / 2, 55 + GetAppropriateFontSize(WIDTH, HEIGHT, 35),
					answer->Meaning, (int)_tcslen(answer->Meaning));
				break;
			}

			if (g_IsWrong) {
				SetTextAlign(dc, TA_LEFT);
				SetTextColor(dc, RGB(255, 0, 0));
				DrawTextUsingFont(dc, GlobalBoldFont, 10, 10 + (g_QuestionOption->ExcludeDuplicatedAnswer ? 40 : 0), STRING("����!"));
				SetTextColor(dc, RGB(0, 0, 0));
			}
		}

		if (g_QuestionOption->ExcludeDuplicatedAnswer) {
			SetTextAlign(dc, TA_LEFT);
			DrawTextUsingFont(dc, GlobalBoldFont, 10, 10, STRING("���� ���� ��"));

			int sum = 0;
			for (int i = 0; i < ARRAYSIZE(QuestionTypes); ++i) {
				sum += g_UnusedVocabulary[i].Count;
			}

			TCHAR count[11 + ARRAYSIZE(_T("��"))] = { 0 };
			_itot(sum - 1, count, 10);
			_tcscat(count, _T("��"));
			DrawTextUsingFont(dc, GlobalDefaultFont, 10, 30, count, (int)_tcslen(count));
		}

		return ENDPAINT;
	}

	case WM_COMMAND: {
		const WORD menu = LOWORD(wParam);
		if (menu == 5) {
			if (MessageBox(handle, _T("���濡�� ���� ������ ��û�Ͻðڽ��ϱ�?"), _T("����"), MB_YESNO | MB_ICONQUESTION) == IDYES) {
				RequestChangeRole(g_Multiplay);
			}
		} else if (menu == 6) {
			SendMessage(handle, WM_USER + 7, 0, 0);
		} else if (!g_Multiplay || g_Multiplay->Option->Role == Examinee) {
			if (menu != g_Question.Answer) {
				g_Question.Words[g_Question.Answer]->IsWrong = g_Question.Words[menu]->IsWrong = g_IsWrong = true;
				InvalidateRect(handle, NULL, FALSE);
				EnableWindow(g_Buttons[menu], FALSE);
				return 0;
			}

			if (!g_IsWrong && g_QuestionOption->ExcludeDuplicatedAnswer) {
				int questionType;
				for (int i = 0; i < ARRAYSIZE(QuestionTypes); ++i) {
					if (QuestionTypes[i] == g_Question.Type) {
						questionType = i;
						break;
					}
				}
				RemoveEqualWord(g_UnusedVocabulary + questionType, g_Question.Words[g_Question.Answer]);
			}

			g_IsWrong = false;
			if (g_Multiplay) {
				SendAnswer(g_Multiplay);
			} else {
				ShowNextQuestion(handle, true);
			}
		} else {
			SendQuestion(g_Multiplay, g_Buttons, menu);
		}
		return 0;
	}

	case WM_USER:
		g_QuestionOption = (QuestionOption*)lParam;
		if (g_QuestionOption->ExcludeDuplicatedAnswer) {
			for (int i = 0; i < ARRAYSIZE(QuestionTypes); ++i) {
				if (QuestionTypes[i] & g_QuestionOption->QuestionType) {
					CopyVocabulary(g_UnusedVocabulary + i, &g_QuestionOption->Vocabulary);
				}
			}
		}

		if (g_QuestionOption->Vocabulary.Count) {
			ShowNextQuestion(handle, true);
		}
		return 0;

	case WM_USER + 1:
		for (int i = 0; i < 5; ++i) {
			ShowWindow(g_Buttons[i], SW_HIDE);
		}

		ShowWindow(g_StopButton, SW_HIDE);

		StartOnlineMultiplay(g_Multiplay = calloc(1, sizeof(OnlineMultiplay)), (OnlineMultiplayOption*)lParam, &g_Question, g_QuestionOption, handle);
		return 0;

	case WM_USER + 2:
		for (int i = 0; i < 5; ++i) {
			ShowWindow(g_Buttons[i], SW_SHOW);
			EnableWindow(g_Buttons[i], g_Multiplay->Option->Role == Examiner);
		}

		if (g_Multiplay->Option->Mode == FixedMode) {
			CreateChangeRoleButton(handle, windowSize);
		}
		ShowWindow(g_StopButton, SW_SHOW);

		if (g_Multiplay->Option->Role == Examiner) {
			if (g_Multiplay->Option->Mode == TurnMode) {
				ShowNextQuestion(handle, true);
			} else {
				SendMessage(handle, WM_USER + 8, 0, 0);
				return 0;
			}
		} else {
			for (int i = 0; i < 5; ++i) {
				SetWindowText(g_Buttons[i], NULL);
			}

			g_Question.Answer = -1;
		}

		g_Multiplay->Status = Connected;
		InvalidateRect(handle, NULL, FALSE);
		return 0;

	case WM_USER + 3:
		ShowNextQuestion(handle, false);
		return 0;

	case WM_USER + 4:
		for (int i = 0; i < 5; ++i) {
			SetWindowText(g_Buttons[i], NULL);
			EnableWindow(g_Buttons[i], g_Multiplay->Option->Mode == TurnMode && g_Multiplay->Option->Role == Examinee);
		}

		g_Question.Answer = -1;
		if (g_Multiplay->Option->Mode == TurnMode) {
			g_Multiplay->Option->Role = g_Multiplay->Option->Role == Examiner ? Examinee : Examiner;
			if (g_Multiplay->Option->Role == Examiner) {
				ShowNextQuestion(handle, true);
			}
		}

		g_Multiplay->Status = Connected;
		InvalidateRect(handle, NULL, FALSE);
		return 0;

	case WM_USER + 5:
		MessageBox(handle, _T("����� ����ϴ� �� ������ �߻��߽��ϴ�."), _T("����"), MB_OK | MB_ICONERROR);
		SendMessage(handle, WM_USER + 7, 0, 0);
		return 0;

	case WM_USER + 6:
		MessageBox(handle, _T("������ ������ �����߽��ϴ�."), _T("����"), MB_OK | MB_ICONINFORMATION);
		SendMessage(handle, WM_USER + 7, 0, 0);
		return 0;

	case WM_USER + 7: {
		Vocabulary* const vocabulary = malloc(sizeof(Vocabulary));
		*vocabulary = g_QuestionOption->Vocabulary;
		g_QuestionOption->Vocabulary.Array = NULL;
		g_QuestionOption->Vocabulary.Count = 0;

		const HWND statisticWindow = CreateAndShowWindow(_T("StatisticWindow"), g_Multiplay ? _T("�¶��� ��Ƽ �÷���") : _T("�ܾ� �ϱ��ϱ�"), SW_SHOW);
		SendMessage(statisticWindow, WM_USER, 0, (LPARAM)vocabulary);

		g_ShouldEnableMainWindow = false;
		SendMessage(handle, WM_CLOSE, 0, 0);
		return 0;
	}

	case WM_USER + 8: {
		const HWND examinerWindow = CreateAndShowWindow(_T("ExaminerWindow"), _T("�¶��� ��Ƽ �÷���"), SW_SHOW);
		SendMessage(examinerWindow, WM_USER + 9, 0, (LPARAM)g_Multiplay);

		g_Multiplay = NULL;
		g_QuestionOption = NULL;

		g_ShouldEnableMainWindow = false;
		SendMessage(handle, WM_CLOSE, 0, 0);
		return 0;
	}

	case WM_USER + 9: {
		g_Multiplay = (OnlineMultiplay*)lParam;
		g_Multiplay->Option->Role = Examinee;
		g_Multiplay->Window = handle;
		g_Multiplay->Question = &g_Question;
		g_QuestionOption = g_Multiplay->QuestionOption;

		CreateChangeRoleButton(handle, windowSize);
		SendMessage(handle, WM_USER + 4, 0, 0);
		return 0;
	}

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

void CreateChangeRoleButton(HWND handle, RECT windowSize) {
	g_ChangeRoleButton = CreateAndShowChild(_T("button"), _T("���� ����\n��û�ϱ�"), g_ButtonFont, BS_PUSHBUTTON | BS_MULTILINE,
		WIDTH - WIDTH / 4 + 10, 140 + ((HEIGHT / 10 + HEIGHT / 50) * 3), WIDTH / 4 - 37, HEIGHT / 10, handle, 5);
}
void ShowNextQuestion(HWND handle, bool generateQuestion) {
	if (generateQuestion) {
		if (g_QuestionOption->ExcludeDuplicatedAnswer) {
			int sum = 0;
			for (int i = 0; i < ARRAYSIZE(QuestionTypes); ++i) {
				sum += g_UnusedVocabulary[i].Count;
			}

			if (sum == 0) {
				MessageBox(handle, _T("��� ������ Ǯ�����ϴ�. �ܾ� �ϱ⸦ �����մϴ�."), _T("����"), MB_OK | MB_ICONINFORMATION);
				SendMessage(handle, WM_USER + 7, 0, 0);
				return;
			}
		}

		GenerateQuestion(&g_Question, g_QuestionOption, NULL, 5, g_QuestionOption->ExcludeDuplicatedAnswer ? g_UnusedVocabulary : NULL);
	}
	SetSelectorText(&g_Question, g_QuestionOption, g_Buttons, 5, g_Multiplay && g_Multiplay->Option->Role == Examiner);
	InvalidateRect(handle, NULL, FALSE);
}