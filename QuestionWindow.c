#include "Window.h"

#include "Multiplay.h"
#include "Word.h"

#include <stdlib.h>
#include <time.h>

static void ShowNextQuestion(HWND handle, bool generateQuestion);

static HFONT g_QuestionFont, g_WordOrMeaningFont, g_PronunciationFont, g_ButtonFont;
static HWND g_Buttons[5], g_StopButton;

static QuestionOption* g_QuestionOption;
static Question g_Question;
static bool g_IsWrong;
static Multiplay* g_Multiplay;

static bool g_ShouldEnableMainWindow = true;

LRESULT CALLBACK QuestionWindowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam) {
	EVENT {
	case WM_CREATE:
		g_QuestionFont = CreateGlobalFont(23, true);
		g_WordOrMeaningFont = CreateGlobalFont(40, true);
		g_PronunciationFont = CreateGlobalFont(28, false);
		g_ButtonFont = CreateGlobalFont(18, false);

		for (int i = 0; i < 5; ++i) {
			g_Buttons[i] = CreateAndShowChild(_T("button"), _T(""), g_ButtonFont, BS_PUSHBUTTON | BS_MULTILINE,
				WIDTH / 2 - WIDTH / 4, 140 + ((HEIGHT / 10 + HEIGHT / 50) * i), WIDTH / 2, HEIGHT / 10, handle, i);
		}
		g_StopButton = CreateAndShowChild(_T("button"), _T("그만 외우기"), g_ButtonFont, BS_PUSHBUTTON,
			WIDTH - WIDTH / 4 + 10, 140 + ((HEIGHT / 10 + HEIGHT / 50) * 4), WIDTH / 4 - 37, HEIGHT / 10, handle, 5);

		g_Question.Answer = -1;
		return 0;

	case WM_DESTROY:
		DeleteObject(g_QuestionFont);
		DeleteObject(g_WordOrMeaningFont);
		DeleteObject(g_PronunciationFont);
		DeleteObject(g_ButtonFont);

		if (g_QuestionOption) {
			DestroyVocabulary(&g_QuestionOption->Vocabulary);
			free(g_QuestionOption);
		}
		g_IsWrong = false;
		if (g_Multiplay) {
			StopMultiplay(g_Multiplay);
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
		DeleteObject(g_ButtonFont);
		g_ButtonFont = CreateGlobalFont(18 * HEIGHT / 480, false);
		for (int i = 0; i < 5; ++i) {
			SetWindowPos(g_Buttons[i], HWND_TOP, WIDTH / 2 - WIDTH / 4, 140 + ((HEIGHT / 10 + HEIGHT / 50) * i), WIDTH / 2, HEIGHT / 10, 0);
			SendMessage(g_Buttons[i], WM_SETFONT, (WPARAM)g_ButtonFont, true);
		}
		SetWindowPos(g_StopButton, HWND_TOP, WIDTH - WIDTH / 4 + 10, 140 + ((HEIGHT / 10 + HEIGHT / 50) * 4), WIDTH / 4 - 37, HEIGHT / 10, 0);
		SendMessage(g_StopButton, WM_SETFONT, (WPARAM)g_ButtonFont, true);
		return 0;

	case WM_PAINT: {
		PAINTSTRUCT ps;
		const HDC dc = BeginPaint(handle, &ps);
		SetTextAlign(dc, TA_CENTER);

		if (g_Multiplay) {
			switch (g_Multiplay->Status) {
			case OpeningServer:
				DrawTextUsingFont(dc, g_QuestionFont, WIDTH / 2, HEIGHT / 2 - 30, STRING("서버를 여는 중..."));
				break;
			case WaitingForPlayer:
				DrawTextUsingFont(dc, g_QuestionFont, WIDTH / 2, HEIGHT / 2 - 30, STRING("상대방을 기다리는 중..."));
				break;
			case PlayerJoining:
				DrawTextUsingFont(dc, g_QuestionFont, WIDTH / 2, HEIGHT / 2 - 30, STRING("상대방이 접속하는 중..."));
				break;
			case JoiningServer:
				DrawTextUsingFont(dc, g_QuestionFont, WIDTH / 2, HEIGHT / 2 - 30, STRING("서버에 접속하는 중..."));
				break;

			case Connected:
			case SentAnswer:
				if (g_Multiplay->Option->Role == Examiner) {
					DrawTextUsingFont(dc, g_QuestionFont, WIDTH / 2, 10, STRING("당신은 출제자입니다."));
					if (g_Multiplay->Status == Connected) {
						DrawTextUsingFont(dc, g_WordOrMeaningFont, WIDTH / 2, 50, STRING("어떤 단어를 정답으로 할까요?"));
					} else {
						DrawTextUsingFont(dc, g_WordOrMeaningFont, WIDTH / 2, 50, STRING("응시자를 기다리는 중..."));
					}
				} else if (g_Question.Answer == -1) {
					DrawTextUsingFont(dc, g_QuestionFont, WIDTH / 2, 10, STRING("당신은 응시자입니다."));
					DrawTextUsingFont(dc, g_WordOrMeaningFont, WIDTH / 2, 50, STRING("출제자를 기다리는 중..."));
				} else goto Default;
				break;
			}
		} else {
			const Word* answer;
		Default:
			answer = g_Question.Words[g_Question.Answer];
			if (g_Question.Type == GuessMeaning) {
				DrawTextUsingFont(dc, g_QuestionFont, WIDTH / 2, 10, STRING("다음 단어의 뜻은?"));
				DrawTextUsingFont(dc, g_WordOrMeaningFont, WIDTH / 2, 50, answer->Word, (int)_tcslen(answer->Word));
				if (g_QuestionOption->GivePronunciation &&
					(answer->Pronunciation[0] == 0 || _tcscmp(answer->Word, answer->Pronunciation))) {
					DrawTextUsingFont(dc, g_QuestionFont, WIDTH / 2, 90, answer->Pronunciation, (int)_tcslen(answer->Pronunciation));
				}
			} else {
				DrawTextUsingFont(dc, g_QuestionFont, WIDTH / 2, 10, STRING("다음 뜻을 가진 단어는?"));
				DrawTextUsingFont(dc, g_WordOrMeaningFont, WIDTH / 2, 50, answer->Meaning, (int)_tcslen(answer->Meaning));
			}

			if (g_IsWrong) {
				SetTextAlign(dc, TA_LEFT);
				SetTextColor(dc, RGB(255, 0, 0));
				DrawTextUsingFont(dc, GlobalBoldFont, 10, 10, STRING("오답!"));
				SetTextColor(dc, RGB(0, 0, 0));
			}
		}

		EndPaint(handle, &ps);
		return 0;
	}

	case WM_COMMAND: {
		const WORD menu = LOWORD(wParam);
		if (menu == 5) {
			SendMessage(handle, WM_USER + 7, 0, 0);
		} else if (!g_Multiplay || g_Multiplay->Option->Role == Examinee) {
			if (menu != g_Question.Answer) {
				g_Question.Words[menu]->IsWrong = g_IsWrong = true;
				InvalidateRect(handle, NULL, TRUE);
				EnableWindow(g_Buttons[menu], FALSE);
				return 0;
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
		if (g_QuestionOption->Vocabulary.Count) {
			ShowNextQuestion(handle, true);
		}
		return 0;

	case WM_USER + 1:
		for (int i = 0; i < 5; ++i) {
			ShowWindow(g_Buttons[i], SW_HIDE);
		}
		ShowWindow(g_StopButton, SW_HIDE);

		StartMultiplay(g_Multiplay = calloc(1, sizeof(Multiplay)), (MultiplayOption*)lParam, &g_Question, g_QuestionOption, handle);
		return 0;

	case WM_USER + 2:
		for (int i = 0; i < 5; ++i) {
			ShowWindow(g_Buttons[i], SW_SHOW);
			EnableWindow(g_Buttons[i], g_Multiplay->Option->Role == Examiner);
		}
		ShowWindow(g_StopButton, SW_SHOW);

		if (g_Multiplay->Option->Role == Examiner) {
			ShowNextQuestion(handle, true);
		} else {
			for (int i = 0; i < 5; ++i) {
				SetWindowText(g_Buttons[i], _T(""));
			}

			g_Question.Answer = -1;
		}

		g_Multiplay->Status = Connected;
		InvalidateRect(handle, NULL, TRUE);
		return 0;

	case WM_USER + 3:
		ShowNextQuestion(handle, false);
		return 0;

	case WM_USER + 4:
		for (int i = 0; i < 5; ++i) {
			SetWindowText(g_Buttons[i], _T(""));
			EnableWindow(g_Buttons[i], g_Multiplay->Option->Mode == TurnMode);
		}

		g_Question.Answer = -1;
		if (g_Multiplay->Option->Mode == TurnMode) {
			g_Multiplay->Option->Role = g_Multiplay->Option->Role == Examiner ? Examinee : Examiner;
			if (g_Multiplay->Option->Role == Examiner) {
				ShowNextQuestion(handle, true);
			}
		}

		g_Multiplay->Status = Connected;
		InvalidateRect(handle, NULL, TRUE);
		return 0;

	case WM_USER + 5:
		MessageBox(handle, _T("상대방과 통신하는 중 오류가 발생했습니다."), _T("오류"), MB_OK | MB_ICONERROR);
		SendMessage(handle, WM_USER + 7, 0, 0);
		return 0;

	case WM_USER + 6:
		MessageBox(handle, _T("상대방이 접속을 종료했습니다."), _T("정보"), MB_OK | MB_ICONINFORMATION);
		SendMessage(handle, WM_USER + 7, 0, 0);
		return 0;

	case WM_USER + 7: {
		Vocabulary* const vocabulary = malloc(sizeof(Vocabulary));
		*vocabulary = g_QuestionOption->Vocabulary;
		g_QuestionOption->Vocabulary.Array = NULL;
		g_QuestionOption->Vocabulary.Count = 0;

		const HWND statisticWindow = CreateAndShowWindow(_T("StatisticWindow"), _T("단어 암기하기"), SW_SHOW);
		SendMessage(statisticWindow, WM_USER, 0, (LPARAM)vocabulary);

		g_ShouldEnableMainWindow = false;
		SendMessage(handle, WM_CLOSE, 0, 0);
		return 0;
	}

	case WM_USER + 8: {
		const HWND examinerWindow = CreateAndShowWindow(_T("ExaminerWindow"), _T("멀티 플레이"), SW_SHOW);
		SendMessage(examinerWindow, WM_USER + 9, 0, (LPARAM)g_Multiplay);

		g_Multiplay = NULL;
		g_QuestionOption = NULL;

		g_ShouldEnableMainWindow = false;
		SendMessage(handle, WM_CLOSE, 0, 0);
		return 0;
	}

	case WM_USER + 9: {
		g_Multiplay = (Multiplay*)lParam;
		g_Multiplay->Option->Role = Examinee;
		g_Multiplay->Window = handle;
		g_Multiplay->Question = &g_Question;
		g_QuestionOption = g_Multiplay->QuestionOption;

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

void ShowNextQuestion(HWND handle, bool generateQuestion) {
	if (generateQuestion) {
		GenerateQuestion(&g_Question, g_QuestionOption, NULL);
	}

	for (int i = 0; i < 5; ++i) {
		if (g_Multiplay && g_Multiplay->Option->Role == Examiner ||
			g_Question.Type == GuessWord) {
			if (g_QuestionOption->GivePronunciation &&
				(g_Question.Words[i]->Pronunciation[0] == 0 || _tcscmp(g_Question.Words[i]->Word, g_Question.Words[i]->Pronunciation))) {
				LPTSTR text = malloc(sizeof(TCHAR) * (_tcslen(g_Question.Words[i]->Word) + _tcslen(g_Question.Words[i]->Pronunciation) + 4));
				_tcscpy(text, g_Question.Words[i]->Word);
				_tcscat(text, _T("\n("));
				_tcscat(text, g_Question.Words[i]->Pronunciation);
				_tcscat(text, _T(")"));
				SetWindowText(g_Buttons[i], text);
				free(text);
			} else {
				SetWindowText(g_Buttons[i], g_Question.Words[i]->Word);
			}
		} else {
			SetWindowText(g_Buttons[i], g_Question.Words[i]->Meaning);
		}
		EnableWindow(g_Buttons[i], TRUE);
	}
	InvalidateRect(handle, NULL, TRUE);
}