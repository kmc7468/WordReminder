#include "Window.h"

#include "Multiplay.h"
#include "Word.h"

#include <stdlib.h>
#include <time.h>

static void ShowNextQuestion(HWND handle, bool generateQuestion);
static void PrepareMultiplay(HWND handle);
static void PrepareNextTurn(HWND handle);

static HFONT g_QuestionFont, g_WordOrMeaningFont, g_PronunciationFont, g_ButtonFont;
static HWND g_Buttons[5], g_StopButton;

static QuestionOption* g_QuestionOption;
static Question g_Question;
static bool g_IsWrong;

static Multiplay g_Multiplay;
static enum {
	Singleplay,
	WaitingForPlayer,
	PlayerJoining,
	JoiningServer,
	Connected,
	SentAnswer,
} g_MultiplayStatus;

#define MAGIC_START 0x11111111
#define MAGIC_COMPATIBLE 0x22222222
#define MAGIC_UNCOMPATIBLE 0x22222223
#define MAGIC_READY 0x33333333
#define MAGIC_QUESTION 0x44444444
#define MAGIC_ANSWER 0x55555555
#define MAGIC_STOP 0x66666666

static Thread g_Thread;
static DWORD WINAPI WaitForPlayerThread(LPVOID param);
static DWORD WINAPI JoinServerThread(LPVOID param);
static DWORD WINAPI ReceiveThread(LPVOID param);

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

		DestroyVocabulary(&g_QuestionOption->Vocabulary);
		free(g_QuestionOption);
		g_IsWrong = false;

		if (g_MultiplayStatus != Singleplay) {
			SendInt(&g_Multiplay, MAGIC_STOP);
			DestroyMultiplay(&g_Multiplay);
		}
		g_MultiplayStatus = Singleplay;

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

		if (g_MultiplayStatus != Singleplay) {
			switch (g_MultiplayStatus) {
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
			SendMessage(handle, WM_USER + 5, 0, 0);
		} else if (g_MultiplayStatus == Singleplay || g_Multiplay.Option->Role == Examinee) {
			if (menu != g_Question.Answer) {
				g_Question.Words[menu]->IsWrong = g_IsWrong = true;
				InvalidateRect(handle, NULL, TRUE);
				EnableWindow(g_Buttons[menu], FALSE);
				return 0;
			}

			g_IsWrong = false;
			if (g_MultiplayStatus == Singleplay) {
				ShowNextQuestion(handle, true);
			} else {
				if (!SendInt(&g_Multiplay, MAGIC_ANSWER)) {
					SendMessage(handle, WM_USER + 3, 0, 0);
					return 0;
				}
				PrepareNextTurn(handle);
			}
		} else {
			if (!SendInt(&g_Multiplay, MAGIC_QUESTION) ||
				!SendInt(&g_Multiplay, g_Question.Type)) {
				SendMessage(handle, WM_USER + 3, 0, 0);
				return 0;
			}
			for (int i = 0; i < 5; ++i) {
				EnableWindow(g_Buttons[i], FALSE);
				if (!SendInt(&g_Multiplay, (int)(g_Question.Words[i] - g_QuestionOption->Vocabulary.Array))) {
					SendMessage(handle, WM_USER + 3, 0, 0);
					return 0;
				}
			}
			if (!SendInt(&g_Multiplay, (g_Question.Answer = menu))) {
				SendMessage(handle, WM_USER + 3, 0, 0);
				return 0;
			}

			g_MultiplayStatus = SentAnswer;
			InvalidateRect(handle, NULL, TRUE);
		}
		return 0;
	}

	case WM_USER:
		g_QuestionOption = (QuestionOption*)lParam;
		ShowNextQuestion(handle, true);
		return 0;

	case WM_USER + 1:
		SetWindowText(handle, _T("멀티 플레이"));

		for (int i = 0; i < 5; ++i) {
			ShowWindow(g_Buttons[i], SW_HIDE);
		}
		ShowWindow(g_StopButton, SW_HIDE);

		g_MultiplayStatus = WaitingForPlayer;
		if (!OpenServer(&g_Multiplay, (MultiplayOption*)lParam)) {
			MessageBox(handle, _T("서버를 여는 중 오류가 발생했습니다."), _T("오류"), MB_OK | MB_ICONERROR);
			SendMessage(handle, WM_CLOSE, 0, 0);
			return 0;
		}
		g_Multiplay.Option->Vocabulary = &g_QuestionOption->Vocabulary;
		StartThread(&g_Thread, WaitForPlayerThread, handle);
		return 0;

	case WM_USER + 2:
		for (int i = 0; i < 5; ++i) {
			ShowWindow(g_Buttons[i], SW_HIDE);
		}
		ShowWindow(g_StopButton, SW_HIDE);

		g_QuestionOption = calloc(1, sizeof(QuestionOption));

		g_MultiplayStatus = JoiningServer;
		if (!JoinServer(&g_Multiplay, (MultiplayOption*)lParam)) {
			MessageBox(handle, _T("서버에 접속하는 중 오류가 발생했습니다."), _T("오류"), MB_OK | MB_ICONERROR);
			SendMessage(handle, WM_CLOSE, 0, 0);
			return 0;
		}
		g_Multiplay.Option->Vocabulary = &g_QuestionOption->Vocabulary;
		StartThread(&g_Thread, JoinServerThread, handle);
		return 0;

	case WM_USER + 3:
		MessageBox(handle, _T("상대방과 통신하는 중 오류가 발생했습니다."), _T("오류"), MB_OK | MB_ICONERROR);
		SendMessage(handle, WM_USER + 5, 0, 0);
		return 0;

	case WM_USER + 4:
		MessageBox(handle, _T("상대방이 접속을 종료했습니다."), _T("정보"), MB_OK | MB_ICONINFORMATION);
		SendMessage(handle, WM_USER + 5, 0, 0);
		return 0;

	case WM_USER + 5: {
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
	srand((unsigned)time(NULL));

	while (!WaitForPlayer(&g_Multiplay));
	return ReceiveThread(param);
}
DWORD WINAPI JoinServerThread(LPVOID param) {
	srand((unsigned)time(NULL));

	LPTSTR serverVersion = NULL;
	int protocolVersion;
	if (!SendInt(&g_Multiplay, MAGIC_START) ||
		!ReceiveVersion(&g_Multiplay, &serverVersion, &protocolVersion)) {
		free(serverVersion);
		goto Error;
	} else if (protocolVersion != WR_MULTIPLAY_PROTOCOL_VERSION) {
		const LPTSTR message = malloc((_tcslen(serverVersion) + 97) * sizeof(TCHAR));
		_tcscpy(message, _T("서버의 버전과 클라이언트의 버전이 호환되지 않습니다.\n"));
		_tcscat(message, serverVersion);
		_tcscat(message, _T(" 버전의 단어 암기 프로그램을 설치해 보세요."));
		MessageBox((HWND)param, message, _T("오류"), MB_OK | MB_ICONERROR);
		free(serverVersion);
		free(message);

		SendInt(&g_Multiplay, MAGIC_UNCOMPATIBLE);
		SendMessage((HWND)param, WM_CLOSE, 0, 0);
		return 0;
	}

	free(serverVersion);
	if (!SendInt(&g_Multiplay, MAGIC_COMPATIBLE) ||
		!ReceiveInt(&g_Multiplay, (int*)&g_QuestionOption->QuestionType) ||
		!ReceiveBool(&g_Multiplay, &g_QuestionOption->GivePronunciation) ||
		!ReceiveInt(&g_Multiplay, (int*)&g_Multiplay.Option->Mode) ||
		!ReceiveInt(&g_Multiplay, (int*)&g_Multiplay.Option->Role) ||
		!ReceiveVocabulary(&g_Multiplay) ||
		!SendInt(&g_Multiplay, MAGIC_READY)) {
	Error:
		MessageBox((HWND)param, _T("서버에 접속하는 중 오류가 발생했습니다."), _T("오류"), MB_OK | MB_ICONERROR);
		SendMessage((HWND)param, WM_CLOSE, 0, 0);
		return 0;
	} else {
		PrepareMultiplay((HWND)param);
		return ReceiveThread(param);
	}
}
DWORD WINAPI ReceiveThread(LPVOID param) {
	int magic;
	while (ReceiveInt(&g_Multiplay, &magic)) {
		if (magic == MAGIC_START) {
			g_MultiplayStatus = PlayerJoining;
			InvalidateRect((HWND)param, NULL, TRUE);
			if (!SendVersion(&g_Multiplay)) goto ConnectionError;
		} else if (magic == MAGIC_COMPATIBLE) {
			if (!SendInt(&g_Multiplay, g_QuestionOption->QuestionType) ||
				!SendBool(&g_Multiplay, g_QuestionOption->GivePronunciation) ||
				!SendInt(&g_Multiplay, g_Multiplay.Option->Mode) ||
				!SendInt(&g_Multiplay, g_Multiplay.Option->Role == Examiner ? Examinee : Examiner) ||
				!SendVocabulary(&g_Multiplay)) goto ConnectionError;
		} else if (magic == MAGIC_UNCOMPATIBLE) {
		ConnectionError:
			g_MultiplayStatus = WaitingForPlayer;
			InvalidateRect((HWND)param, NULL, TRUE);
			return WaitForPlayerThread(param);
		} else if (magic == MAGIC_READY) {
			PrepareMultiplay((HWND)param);
		} else if (magic == MAGIC_QUESTION) {
			if (!ReceiveInt(&g_Multiplay, (int*)&g_Question.Type)) goto Error;
			for (int i = 0; i < 5; ++i) {
				int index;
				if (!ReceiveInt(&g_Multiplay, &index)) goto Error;
				g_Question.Words[i] = g_QuestionOption->Vocabulary.Array + index;
			}
			if (!ReceiveInt(&g_Multiplay, &g_Question.Answer)) goto Error;
			ShowNextQuestion((HWND)param, false);
		} else if (magic == MAGIC_ANSWER) {
			PrepareNextTurn((HWND)param);
		} else {
			SendMessage((HWND)param, WM_USER + 4, 0, 0);
			return 0;
		}
	}

Error:
	SendMessage((HWND)param, WM_USER + 3, 0, 0);
	return 0;
}

void ShowNextQuestion(HWND handle, bool generateQuestion) {
	if (generateQuestion) {
		GenerateQuestion(&g_Question, g_QuestionOption);
	}

	for (int i = 0; i < 5; ++i) {
		if (g_MultiplayStatus != Singleplay && g_Multiplay.Option->Role == Examiner ||
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
void PrepareMultiplay(HWND handle) {
	for (int i = 0; i < 5; ++i) {
		ShowWindow(g_Buttons[i], SW_SHOW);
		EnableWindow(g_Buttons[i], g_Multiplay.Option->Role == Examiner);
	}
	ShowWindow(g_StopButton, SW_SHOW);

	if (g_Multiplay.Option->Role == Examiner) {
		ShowNextQuestion(handle, true);
	} else {
		for (int i = 0; i < 5; ++i) {
			SetWindowText(g_Buttons[i], _T(""));
		}
	}

	g_MultiplayStatus = Connected;
	InvalidateRect(handle, NULL, TRUE);
}
void PrepareNextTurn(HWND handle) {
	for (int i = 0; i < 5; ++i) {
		SetWindowText(g_Buttons[i], _T(""));
	}

	g_Question.Answer = -1;
	if (g_Multiplay.Option->Role == Examiner) {
		g_Multiplay.Option->Role = Examinee;
	} else {
		g_Multiplay.Option->Role = Examiner;
		ShowNextQuestion(handle, true);
	}

	g_MultiplayStatus = Connected;
	InvalidateRect(handle, NULL, TRUE);
}