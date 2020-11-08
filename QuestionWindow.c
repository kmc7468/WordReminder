#include "Window.h"

#include "Multiplay.h"
#include "Word.h"

#include <stdlib.h>

static QuestionOption* g_QuestionOption;
static Question g_Question;
static bool g_IsWrong;

typedef enum {
	None,
	WaitingForPlayer,
	JoiningPlayer,
	JoiningServer,
	Connected,
	AnswerSent,
} MultiplayStatus;

static Multiplay g_Multiplay;
static MultiplayStatus g_MultiplayStatus;

static HANDLE g_Thread;
static DWORD g_ThreadId;
static DWORD WINAPI WaitForPlayerThread(LPVOID param);
static DWORD WINAPI JoinServerThread(LPVOID param);
static DWORD WINAPI WaitForAnswerThread(LPVOID param);
static DWORD WINAPI WaitForQuestionThread(LPVOID param);

static HFONT g_QuestionFont, g_WordOrMeaningFont, g_PronunciationFont, g_ButtonFont;
static HWND g_Buttons[5];
static HWND g_StopButton;

static bool g_ShouldEnableMainWindow = true;

static void ShowNextQuestion(HWND handle, bool generate);

LRESULT CALLBACK QuestionWindowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam) {
	EVENT {
	case WM_CREATE:
		g_Question.Answer = -1;

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
		return 0;

	case WM_DESTROY:
		if (g_MultiplayStatus != None) {
			SendInt(&g_Multiplay, 0x34343434);
		}

		DestroyVocabulary(&g_QuestionOption->Vocabulary);
		free(g_QuestionOption);
		g_IsWrong = false;

		DestroyMultiplay(&g_Multiplay);
		g_MultiplayStatus = None;

		DeleteObject(g_QuestionFont);
		DeleteObject(g_WordOrMeaningFont);
		DeleteObject(g_PronunciationFont);
		DeleteObject(g_ButtonFont);

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

		if (g_MultiplayStatus != None) {
			switch (g_MultiplayStatus) {
			case WaitingForPlayer:
				DrawTextUsingFont(dc, g_QuestionFont, WIDTH / 2, HEIGHT / 2 - 30, STRING("상대방을 기다리는 중..."));
				break;
			case JoiningPlayer:
				DrawTextUsingFont(dc, g_QuestionFont, WIDTH / 2, HEIGHT / 2 - 30, STRING("상대방이 접속하는 중..."));
				break;
			case JoiningServer:
				DrawTextUsingFont(dc, g_QuestionFont, WIDTH / 2, HEIGHT / 2 - 30, STRING("서버에 접속하는 중..."));
				break;

			case Connected:
			case AnswerSent:
				if (g_Multiplay.Option->Role == Examiner) {
					DrawTextUsingFont(dc, g_QuestionFont, WIDTH / 2, 10, STRING("당신은 출제자입니다."));
					if (g_MultiplayStatus == Connected) {
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
			if (g_Question.Type == GuessingMeaning) {
				DrawTextUsingFont(dc, g_QuestionFont, WIDTH / 2, 10, STRING("다음 단어의 뜻은?"));
				DrawTextUsingFont(dc, g_WordOrMeaningFont, WIDTH / 2, 50, answer->Word, (int)_tcslen(answer->Word));
				if (g_QuestionOption->ShouldGivePronunciation &&
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

	case WM_COMMAND:
		if (LOWORD(wParam) < 5) {
			if (g_MultiplayStatus == None || g_Multiplay.Option->Role == Examinee) {
				if (LOWORD(wParam) == g_Question.Answer) {
					g_IsWrong = false;
					if (g_MultiplayStatus != None) {
						SendInt(&g_Multiplay, 0x12121212);
						g_Multiplay.Option->Role = g_Multiplay.Option->Role == Examiner ? Examinee : Examiner;
						SendMessage(handle, WM_USER + 6, 0, 0);
					} else {
						ShowNextQuestion(handle, true);
					}
				} else {
					g_Question.Words[g_Question.Answer]->IsWrong = g_IsWrong = true;
					EnableWindow(g_Buttons[LOWORD(wParam)], FALSE);
					InvalidateRect(handle, NULL, TRUE);
				}
			} else {
				SendInt(&g_Multiplay, g_Question.Type);
				for (int i = 0; i < 5; ++i) {
					EnableWindow(g_Buttons[i], FALSE);
					SendInt(&g_Multiplay, g_Question.Words[i] - g_QuestionOption->Vocabulary.Array);
				}
				SendInt(&g_Multiplay, (g_Question.Answer = LOWORD(wParam)));
				g_Thread = CreateThread(NULL, 0, WaitForAnswerThread, handle, 0, &g_ThreadId);
				g_MultiplayStatus = AnswerSent;
				InvalidateRect(handle, NULL, TRUE);
			}
		} else {
			SendMessage(handle, WM_USER + 8, 0, 0);
		}
		return 0;

	case WM_USER:
		g_QuestionOption = (QuestionOption*)lParam;
		ShowNextQuestion(handle, true);
		return 0;

	case WM_USER + 1:
		g_MultiplayStatus = WaitingForPlayer;
		SetWindowText(handle, _T("멀티 플레이"));
		for (int i = 0; i < 5; ++i) {
			ShowWindow(g_Buttons[i], SW_HIDE);
		}
		ShowWindow(g_StopButton, SW_HIDE);

		if (!OpenServer(&g_Multiplay, (MultiplayOption*)lParam)) {
			MessageBox(handle, _T("서버를 여는데 실패했습니다."), _T("오류"), MB_OK | MB_ICONERROR);
			SendMessage(handle, WM_CLOSE, 0, 0);
			break;
		}
		g_Multiplay.Option->Vocabulary = &g_QuestionOption->Vocabulary;
		g_Thread = CreateThread(NULL, 0, WaitForPlayerThread, handle, 0, &g_ThreadId);
		break;

	case WM_USER + 2:
		if (lParam == 0) {
			if (g_MultiplayStatus == WaitingForPlayer) {
				MessageBox(handle, _T("상대방을 기다리는 중 오류가 발생했습니다."), _T("오류"), MB_OK | MB_ICONERROR);
				SendMessage(handle, WM_CLOSE, 0, 0);
			} else {
				MessageBox(handle, _T("상대방이 접속하는 중 오류가 발생했습니다."), _T("오류"), MB_OK | MB_ICONERROR);
				SendMessage(handle, WM_CLOSE, 0, 0);
			}
		} else {
			SendMessage(handle, WM_USER + 5, 0, 0);
		}
		break;

	case WM_USER + 3:
		g_QuestionOption = calloc(1, sizeof(QuestionOption));
		if (!g_QuestionOption) {
			MessageBox(handle, _T("메모리가 부족합니다."), _T("오류"), MB_OK | MB_ICONERROR);
			SendMessage(handle, WM_CLOSE, 0, 0);
			break;
		}

		g_MultiplayStatus = JoiningServer;
		for (int i = 0; i < 5; ++i) {
			ShowWindow(g_Buttons[i], SW_HIDE);
		}
		ShowWindow(g_StopButton, SW_HIDE);

		if (!JoinServer(&g_Multiplay, (MultiplayOption*)lParam)) {
			MessageBox(handle, _T("서버에 접속하는데 실패했습니다."), _T("오류"), MB_OK | MB_ICONERROR);
			SendMessage(handle, WM_CLOSE, 0, 0);
			break;
		}
		g_Multiplay.Option->Vocabulary = &g_QuestionOption->Vocabulary;
		g_Thread = CreateThread(NULL, 0, JoinServerThread, handle, 0, &g_ThreadId);
		break;

	case WM_USER + 4:
		if (lParam == 0) {
			MessageBox(handle, _T("서버에 접속하는데 실패했습니다."), _T("오류"), MB_OK | MB_ICONERROR);
			SendMessage(handle, WM_CLOSE, 0, 0);
		} else {
			SendMessage(handle, WM_USER + 5, 0, 0);
		}
		break;

	case WM_USER + 5:
		g_MultiplayStatus = Connected;
		for (int i = 0; i < 5; ++i) {
			ShowWindow(g_Buttons[i], SW_SHOW);
			EnableWindow(g_Buttons[i], g_Multiplay.Option->Role == Examiner);
		}
		ShowWindow(g_StopButton, SW_SHOW);
		SendMessage(handle, WM_USER + 6, 0, 0);
		return 0;

	case WM_USER + 6:
		if (g_Multiplay.Option->Role == Examiner) {
			ShowNextQuestion(handle, true);
		} else {
			g_Thread = CreateThread(NULL, 0, WaitForQuestionThread, handle, 0, &g_ThreadId);
			InvalidateRect(handle, NULL, TRUE);
		}
		break;

	case WM_USER + 7:
		MessageBox(handle, _T("상대방이 접속을 종료했습니다."), _T("정보"), MB_OK | MB_ICONINFORMATION);
	case WM_USER + 8: {
		Vocabulary* const vocabulary = malloc(sizeof(Vocabulary));
		*vocabulary = g_QuestionOption->Vocabulary;
		g_QuestionOption->Vocabulary.Array = NULL;
		g_QuestionOption->Vocabulary.Count = 0;

		const HWND statisticWindow = CreateAndShowWindow(_T("StatisticWindow"), _T("단어 암기하기"), SW_SHOW);
		SendMessage(statisticWindow, WM_USER, 0, (LPARAM)vocabulary);

		g_ShouldEnableMainWindow = false;
		SendMessage(handle, WM_CLOSE, 0, 0);
		break;
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

DWORD WINAPI WaitForPlayerThread(LPVOID param) {
	if (!WaitForPlayer(&g_Multiplay)) {
		SendMessage((HWND)param, WM_USER + 2, 0, 0);
		return 0;
	}

	int dummy;
	g_MultiplayStatus = JoiningPlayer;
	InvalidateRect((HWND)param, NULL, TRUE);
	if (!SendInt(&g_Multiplay, g_QuestionOption->QuestionType) ||
		!SendBool(&g_Multiplay, g_QuestionOption->ShouldGivePronunciation) ||
		!SendInt(&g_Multiplay, g_Multiplay.Option->Mode) ||
		!SendInt(&g_Multiplay, g_Multiplay.Option->Role == Examiner ? Examinee : Examiner) ||
		!SendVocabulary(&g_Multiplay) ||
		!ReceiveInt(&g_Multiplay, &dummy)) {
		SendMessage((HWND)param, WM_USER + 2, 0, 0);
		return 0;
	} else {
		SendMessage((HWND)param, WM_USER + 2, 0, 1);
		return 0;
	}
}
DWORD WINAPI JoinServerThread(LPVOID param) {
	if (!ReceiveInt(&g_Multiplay, (int*)&g_QuestionOption->QuestionType) ||
		!ReceiveBool(&g_Multiplay, &g_QuestionOption->ShouldGivePronunciation) ||
		!ReceiveInt(&g_Multiplay, (int*)&g_Multiplay.Option->Mode) ||
		!ReceiveInt(&g_Multiplay, (int*)&g_Multiplay.Option->Role) ||
		!ReceiveVocabulary(&g_Multiplay) ||
		!SendInt(&g_Multiplay, 0x12345678)) {
		SendMessage((HWND)param, WM_USER + 4, 0, 0);
		return 0;
	} else {
		SendMessage((HWND)param, WM_USER + 4, 0, 1);
		return 0;
	}
}
DWORD WINAPI WaitForAnswerThread(LPVOID param) {
	int dummy;
	ReceiveInt(&g_Multiplay, &dummy);
	if (dummy == 0x34343434) {
		SendMessage((HWND)param, WM_USER + 7, 0, 0);
		return 0;
	}

	g_Multiplay.Option->Role = g_Multiplay.Option->Role == Examiner ? Examinee : Examiner;
	g_MultiplayStatus = Connected;
	g_Question.Answer = -1;
	for (int i = 0; i < 5; ++i) {
		SetWindowText(g_Buttons[i], _T(""));
	}
	SendMessage((HWND)param, WM_USER + 6, 0, 0);
	return 0;
}
DWORD WINAPI WaitForQuestionThread(LPVOID param) {
	ReceiveInt(&g_Multiplay, (int*)&g_Question.Type);
	if (g_Question.Type == 0x34343434) {
		SendMessage((HWND)param, WM_USER + 7, 0, 0);
		return 0;
	}

	for (int i = 0; i < 5; ++i) {
		int index;
		ReceiveInt(&g_Multiplay, &index);
		g_Question.Words[i] = g_QuestionOption->Vocabulary.Array + index;
	}
	ReceiveInt(&g_Multiplay, &g_Question.Answer);
	ShowNextQuestion((HWND)param, false);
	return 0;
}

void ShowNextQuestion(HWND handle, bool generate) {
	if (generate) {
		GenerateQuestion(&g_Question, g_QuestionOption);
	}

	for (int i = 0; i < 5; ++i) {
		if (g_MultiplayStatus != None && g_Multiplay.Option->Role == Examiner ||
			g_Question.Type == GuessingWord) {
			if (g_QuestionOption->ShouldGivePronunciation &&
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