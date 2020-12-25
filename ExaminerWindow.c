#include "Window.h"

#include "Multiplay.h"
#include "Word.h"

#include <stdlib.h>

static void CreateChildren(HWND handle, RECT windowSize);

static HFONT g_RoleFont, g_StatusFont;
static HWND g_WordList;
static HWND g_WordEdit, g_PronunciationEdit, g_MeaningEdit;
static HWND g_SendButton, g_ChangeRoleButton, g_StopButton;

static QuestionOption* g_QuestionOption;
static Question g_Question;
static OnlineMultiplay* g_Multiplay;

static bool g_ShouldEnableMainWindow = true;

LRESULT CALLBACK ExaminerWindowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam) {
	EVENT {
	case WM_CREATE:
		g_RoleFont = CreateGlobalFont(23, true);
		g_StatusFont = CreateGlobalFont(40, true);

		g_Multiplay = malloc(sizeof(OnlineMultiplay));
		return 0;

	case WM_DESTROY:
		DeleteObject(g_RoleFont);
		DeleteObject(g_StatusFont);

		if (g_QuestionOption) {
			DestroyVocabulary(&g_QuestionOption->Vocabulary);
			free(g_QuestionOption);
		}
		if (g_Multiplay) {
			StopOnlineMultiplay(g_Multiplay);
			free(g_Multiplay);
		}

		if (g_ShouldEnableMainWindow) {
			EnableWindow(MainWindow, TRUE);
			SetWindowPos(MainWindow, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}
		g_ShouldEnableMainWindow = true;
		return 0;

	case WM_SIZE:
		SetWindowPos(g_WordList, HWND_TOP, 0, 0, WIDTH / 3, HEIGHT - 172, SWP_NOMOVE);

		SetWindowPos(g_WordEdit, HWND_TOP, WIDTH / 3 + 20, 145, WIDTH / 3 * 2 - 45, 25, 0);
		SetWindowPos(g_PronunciationEdit, HWND_TOP, WIDTH / 3 + 20, 205, WIDTH / 3 * 2 - 45, 25, 0);
		SetWindowPos(g_MeaningEdit, HWND_TOP, WIDTH / 3 + 20, 265, WIDTH / 3 * 2 - 45, 25, 0);

		SetWindowPos(g_SendButton, HWND_TOP, WIDTH / 3 + 20, 300, WIDTH / 3 * 2 - 45, 50, 0);
		SetWindowPos(g_ChangeRoleButton, HWND_TOP, WIDTH / 3 + 20, HEIGHT - 103, WIDTH / 3 - 25, 50, 0);
		SetWindowPos(g_StopButton, HWND_TOP, WIDTH / 3 * 2 + 1, HEIGHT - 103, WIDTH / 3 - 25, 50, 0);
		return 0;

	case WM_PAINT: {
		BEGINPAINT;
		SetTextAlign(dc, TA_CENTER);

		switch (g_Multiplay->Status) {
		case OpeningServer:
			DrawTextUsingFont(dc, g_RoleFont, WIDTH / 2, HEIGHT / 2 - 30, STRING("서버를 여는 중..."));
			break;
		case WaitingForPlayer:
			DrawTextUsingFont(dc, g_RoleFont, WIDTH / 2, HEIGHT / 2 - 30, STRING("상대방을 기다리는 중..."));
			break;
		case PlayerJoining:
			DrawTextUsingFont(dc, g_RoleFont, WIDTH / 2, HEIGHT / 2 - 30, STRING("상대방이 접속하는 중..."));
			break;
		case JoiningServer:
			DrawTextUsingFont(dc, g_RoleFont, WIDTH / 2, HEIGHT / 2 - 30, STRING("서버에 접속하는 중..."));
			break;

		case Connected:
		case SentAnswer:
			DrawTextUsingFont(dc, g_RoleFont, WIDTH / 2, 10, STRING("당신은 출제자입니다."));
			if (g_Multiplay->Status == Connected) {
				DrawTextUsingFont(dc, g_StatusFont, WIDTH / 2, 50, STRING("어떤 단어를 정답으로 할까요?"));
			} else {
				DrawTextUsingFont(dc, g_StatusFont, WIDTH / 2, 50, STRING("응시자를 기다리는 중..."));
			}

			SetTextAlign(dc, TA_LEFT);
			DrawTextUsingFont(dc, GlobalDefaultFont, WIDTH / 3 + 20, 120, STRING("단어"));
			DrawTextUsingFont(dc, GlobalDefaultFont, WIDTH / 3 + 20, 180, STRING("발음"));
			DrawTextUsingFont(dc, GlobalDefaultFont, WIDTH / 3 + 20, 240, STRING("뜻"));
			break;
		}

		return ENDPAINT;
	}

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case 0: {
			if (HIWORD(wParam) != LBN_SELCHANGE) break;

			const int index = (int)SendMessage(g_WordList, LB_GETCURSEL, 0, 0);
			if (index == -1) break;

			SetWindowText(g_WordEdit, g_QuestionOption->Vocabulary.Array[index].Word);
			SetWindowText(g_PronunciationEdit, g_QuestionOption->Vocabulary.Array[index].Pronunciation);
			SetWindowText(g_MeaningEdit, g_QuestionOption->Vocabulary.Array[index].Meaning);
			break;
		}

		case 4: {
			const int index = (int)SendMessage(g_WordList, LB_GETCURSEL, 0, 0);
			if (index == -1) {
				MessageBox(handle, _T("단어를 선택해 주세요."), _T("오류"), MB_OK | MB_ICONERROR);
				break;
			}

			EnableWindow(g_SendButton, FALSE);
			GenerateQuestion(&g_Question, g_QuestionOption, g_QuestionOption->Vocabulary.Array + index, 5, NULL);
			SendQuestion(g_Multiplay, NULL, g_Question.Answer);
			break;
		}

		case 5:
			if (MessageBox(handle, _T("상대방에게 역할 변경을 요청하시겠습니까?"), _T("정보"), MB_YESNO | MB_ICONQUESTION) == IDYES) {
				RequestChangeRole(g_Multiplay);
			}
			break;

		case 6:
			SendMessage(handle, WM_USER + 7, 0, 0);
			break;
		}
		return 0;

	case WM_USER:
		g_QuestionOption = (QuestionOption*)lParam;
		return 0;

	case WM_USER + 1:
		StartOnlineMultiplay(g_Multiplay, (OnlineMultiplayOption*)lParam, &g_Question, g_QuestionOption, handle);
		return 0;

	case WM_USER + 2:
		CreateChildren(handle, windowSize);

		g_Multiplay->Status = Connected;
		InvalidateRect(handle, NULL, FALSE);
		return 0;

	case WM_USER + 4:
		EnableWindow(g_SendButton, TRUE);

		g_Multiplay->Status = Connected;
		InvalidateRect(handle, NULL, FALSE);
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

		const HWND statisticWindow = CreateAndShowWindow(_T("StatisticWindow"), _T("온라인 멀티 플레이"), SW_SHOW);
		SendMessage(statisticWindow, WM_USER, 0, (LPARAM)vocabulary);

		g_ShouldEnableMainWindow = false;
		SendMessage(handle, WM_CLOSE, 0, 0);
		return 0;
	}

	case WM_USER + 8: {
		const HWND questionWindow = CreateAndShowWindow(_T("QuestionWindow"), _T("온라인 멀티 플레이"), SW_SHOW);
		SendMessage(questionWindow, WM_USER + 9, 0, (LPARAM)g_Multiplay);

		g_Multiplay = NULL;
		g_QuestionOption = NULL;

		g_ShouldEnableMainWindow = false;
		SendMessage(handle, WM_CLOSE, 0, 0);
		return 0;
	}

	case WM_USER + 9: {
		g_Multiplay = (OnlineMultiplay*)lParam;
		g_Multiplay->Option->Role = Examiner;
		g_Multiplay->Window = handle;
		g_Multiplay->Question = &g_Question;
		g_QuestionOption = g_Multiplay->QuestionOption;

		CreateChildren(handle, windowSize);
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

void CreateChildren(HWND handle, RECT windowSize) {
	g_WordList = CreateAndShowChild(_T("listbox"), NULL, GlobalDefaultFont, WS_BORDER | WS_VSCROLL | LBS_NOTIFY | LBS_NOINTEGRALHEIGHT,
		10, 120, WIDTH / 3, HEIGHT - 172, handle, 0);
	SendMessage(handle, WM_SIZE, 0, 0);
	for (int i = 0; i < g_QuestionOption->Vocabulary.Count; ++i) {
		SendMessage(g_WordList, LB_ADDSTRING, 0, (LPARAM)g_QuestionOption->Vocabulary.Array[i].Word);
	}

	g_WordEdit = CreateAndShowChild(_T("edit"), NULL, GlobalDefaultFont, WS_BORDER | WS_GROUP | WS_TABSTOP,
		WIDTH / 3 + 20, 145, WIDTH / 3 * 2 - 45, 25, handle, 1);
	g_PronunciationEdit = CreateAndShowChild(_T("edit"), NULL, GlobalDefaultFont, WS_BORDER | WS_GROUP | WS_TABSTOP,
		WIDTH / 3 + 20, 205, WIDTH / 3 * 2 - 45, 25, handle, 2);
	g_MeaningEdit = CreateAndShowChild(_T("edit"), NULL, GlobalDefaultFont, WS_BORDER | WS_GROUP | WS_TABSTOP,
		WIDTH / 3 + 20, 265, WIDTH / 3 * 2 - 45, 25, handle, 3);
	SendMessage(g_WordEdit, EM_SETREADONLY, TRUE, 0);
	SendMessage(g_PronunciationEdit, EM_SETREADONLY, TRUE, 0);
	SendMessage(g_MeaningEdit, EM_SETREADONLY, TRUE, 0);

	g_SendButton = CreateAndShowChild(_T("button"), _T("출제하기"), GlobalBoldFont, BS_PUSHBUTTON,
		WIDTH / 3 + 20, 300, WIDTH / 3 * 2 - 45, 50, handle, 4);
	g_ChangeRoleButton = CreateAndShowChild(_T("button"), _T("역할 변경 요청하기"), GlobalBoldFont, BS_PUSHBUTTON,
		WIDTH / 3 + 20, HEIGHT - 103, WIDTH / 3 - 25, 50, handle, 5);
	g_StopButton = CreateAndShowChild(_T("button"), _T("그만 외우기"), GlobalBoldFont, BS_PUSHBUTTON,
		WIDTH / 3 * 2 + 1, HEIGHT - 103, WIDTH / 3 - 25, 50, handle, 6);
}