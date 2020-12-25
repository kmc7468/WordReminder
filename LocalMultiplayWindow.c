#include "Window.h"

#include "Multiplay.h"
#include "Word.h"

#include <math.h>
#include <stdlib.h>

static void ShowNextQuestion(HWND handle);

static HFONT g_QuestionFont, g_WordOrMeaningFont, g_MeaningOrPronunciationFont, g_ButtonFont;
static HWND g_Buttons[4];
static HWND g_StopButton;

static QuestionOption* g_QuestionOption;
static Question g_Question;
static LocalMultiplay* g_Multiplay;
static int g_Winner;

LRESULT CALLBACK LocalMultiplayWindowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam) {
	EVENT {
	case WM_CREATE:
		g_QuestionFont = CreateGlobalFont(23, true);
		g_WordOrMeaningFont = CreateGlobalFont(40, true);
		g_MeaningOrPronunciationFont = CreateGlobalFont(28, false);
		g_ButtonFont = CreateGlobalFont(18, false);

		g_Buttons[0] = CreateAndShowChild(_T("button"), NULL, g_ButtonFont, BS_PUSHBUTTON | BS_MULTILINE,
			WIDTH / 2 - WIDTH / 8, 140, WIDTH / 4, HEIGHT / 5, handle, 0);
		for (int i = 0; i < 3; ++i) {
			g_Buttons[i + 1] = CreateAndShowChild(_T("button"), NULL, g_ButtonFont, BS_PUSHBUTTON | BS_MULTILINE,
				WIDTH / 2 - WIDTH / 8 + (i - 1) * (WIDTH / 4 + HEIGHT / 50), 140 + HEIGHT / 5 + HEIGHT / 50, WIDTH / 4, HEIGHT / 5, handle, i + 1);
		}

		g_StopButton = CreateAndShowChild(_T("button"), _T("그만 외우기"), g_ButtonFont, BS_PUSHBUTTON,
			WIDTH / 2 - WIDTH / 8, 140 + (HEIGHT / 5 + HEIGHT / 50) * 2, WIDTH / 4, HEIGHT / 10, handle, 4);

		g_Question.Answer = -1;
		g_Multiplay = calloc(1, sizeof(LocalMultiplay));
		g_Winner = 0;
		return 0;

	case WM_DESTROY:
		DeleteObject(g_QuestionFont);
		DeleteObject(g_WordOrMeaningFont);
		DeleteObject(g_MeaningOrPronunciationFont);
		DeleteObject(g_ButtonFont);

		DestroyVocabulary(&g_QuestionOption->Vocabulary);
		free(g_QuestionOption);
		free(g_Multiplay);

		EnableWindow(MainWindow, TRUE);
		SetWindowPos(MainWindow, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		return 0;

	case WM_SIZE:
		DeleteObject(g_WordOrMeaningFont);
		DeleteObject(g_MeaningOrPronunciationFont);
		DeleteObject(g_ButtonFont);
		g_WordOrMeaningFont = CreateGlobalFont(GetAppropriateFontSize(WIDTH, HEIGHT, 40), true);
		g_MeaningOrPronunciationFont = CreateGlobalFont(GetAppropriateFontSize(WIDTH, HEIGHT, 28), false);
		g_ButtonFont = CreateGlobalFont(GetAppropriateFontSize(WIDTH, HEIGHT, 18), false);

		SetWindowPos(g_Buttons[0], HWND_TOP, WIDTH / 2 - WIDTH / 8, 75 + GetAppropriateFontSize(WIDTH, HEIGHT, 35) + GetAppropriateFontSize(WIDTH, HEIGHT, 30), WIDTH / 4, HEIGHT / 5, 0);
		SendMessage(g_Buttons[0], WM_SETFONT, (WPARAM)g_ButtonFont, true);
		for (int i = 0; i < 3; ++i) {
			SetWindowPos(g_Buttons[i + 1], HWND_TOP, WIDTH / 2 - WIDTH / 8 + (i - 1) * (WIDTH / 4 + HEIGHT / 50), 75 + GetAppropriateFontSize(WIDTH, HEIGHT, 35) + GetAppropriateFontSize(WIDTH, HEIGHT, 30) + HEIGHT / 5 + HEIGHT / 50, WIDTH / 4, HEIGHT / 5, 0);
			SendMessage(g_Buttons[i + 1], WM_SETFONT, (WPARAM)g_ButtonFont, true);
		}

		SetWindowPos(g_StopButton, HWND_TOP, WIDTH / 2 - WIDTH / 8, 75 + GetAppropriateFontSize(WIDTH, HEIGHT, 35) + GetAppropriateFontSize(WIDTH, HEIGHT, 30) + (HEIGHT / 5 + HEIGHT / 50) * 2, WIDTH / 4, HEIGHT / 10, 0);
		SendMessage(g_StopButton, WM_SETFONT, (WPARAM)g_ButtonFont, true);
		return 0;

	case WM_PAINT: {
		BEGINPAINT;
		SetTextAlign(dc, TA_CENTER);

		const Word* const answer = g_Question.Words[g_Question.Answer];
		switch (g_Question.Type) {
		case GuessMeaning:
			DrawTextUsingFont(dc, g_QuestionFont, WIDTH / 2, 10, STRING("다음 단어의 뜻은?"));
			DrawTextUsingFont(dc, g_WordOrMeaningFont, WIDTH / 2, 50, answer->Word, (int)_tcslen(answer->Word));
			if (g_QuestionOption->GivePronunciation && answer->Pronunciation[0] != 0 && _tcscmp(answer->Word, answer->Pronunciation)) {
				DrawTextUsingFont(dc, g_MeaningOrPronunciationFont, WIDTH / 2, 55 + GetAppropriateFontSize(WIDTH, HEIGHT, 35),
					answer->Pronunciation, (int)_tcslen(answer->Pronunciation));
			}
			break;

		case GuessWord:
			DrawTextUsingFont(dc, g_QuestionFont, WIDTH / 2, 10, STRING("다음 뜻을 가진 단어는?"));
			DrawTextUsingFont(dc, g_WordOrMeaningFont, WIDTH / 2, 50, answer->Meaning, (int)_tcslen(answer->Meaning));
			break;

		case GuessPronunciation:
			DrawTextUsingFont(dc, g_QuestionFont, WIDTH / 2, 10, STRING("다음 단어의 발음은?"));
			DrawTextUsingFont(dc, g_WordOrMeaningFont, WIDTH / 2, 50, answer->Word, (int)_tcslen(answer->Word));
			DrawTextUsingFont(dc, g_MeaningOrPronunciationFont, WIDTH / 2, 55 + GetAppropriateFontSize(WIDTH, HEIGHT, 35),
				answer->Meaning, (int)_tcslen(answer->Meaning));
			break;
		}

		SetTextAlign(dc, TA_LEFT);
		DrawTextUsingFont(dc, GlobalBoldFont, 10, 10, STRING("왼쪽 플레이어"));
		TCHAR score[11 + ARRAYSIZE(_T("점"))] = { 0 };
		_itot(g_Multiplay->Players[0].Score, score, 10);
		_tcscat(score, _T("점"));
		DrawTextUsingFont(dc, GlobalDefaultFont, 10, 30, score, (int)_tcslen(score));

		SetTextAlign(dc, TA_RIGHT);
		DrawTextUsingFont(dc, GlobalBoldFont, WIDTH - 26, 10, STRING("오른쪽 플레이어"));
		_itot(g_Multiplay->Players[1].Score, score, 10);
		_tcscat(score, _T("점"));
		DrawTextUsingFont(dc, GlobalDefaultFont, WIDTH - 26, 30, score, (int)_tcslen(score));

		if (g_Multiplay->Phase != 0) {
			SetTextAlign(dc, TA_LEFT);
			if (g_Multiplay->Phase == 1 || g_Multiplay->Phase == -2) {
				SetTextColor(dc, RGB(255, 0, 0));
				DrawTextUsingFont(dc, GlobalBoldFont, 10, 50, STRING("오답!"));
				SetTextAlign(dc, TA_RIGHT);
				SetTextColor(dc, RGB(0, 0, 255));
				DrawTextUsingFont(dc, GlobalBoldFont, WIDTH - 26, 50, STRING("당신의 차례입니다"));
			} else {
				SetTextColor(dc, RGB(0, 0, 255));
				DrawTextUsingFont(dc, GlobalBoldFont, 10, 50, STRING("당신의 차례입니다"));
				SetTextAlign(dc, TA_RIGHT);
				SetTextColor(dc, RGB(255, 0, 0));
				DrawTextUsingFont(dc, GlobalBoldFont, WIDTH - 26, 50, STRING("오답!"));
			}
			SetTextColor(dc, RGB(0, 0, 0));
		}

		if (g_Winner) {
			SetTextColor(dc, RGB(255 * (g_Winner < 0), 255 * (g_Winner > 0), 0));
			const LPCTSTR status = g_Winner > 0 ? _T("정답!") : _T("오답!");
			if (abs(g_Winner) == 1) {
				SetTextAlign(dc, TA_LEFT);
				DrawTextUsingFont(dc, GlobalBoldFont, 10, 50, status, (int)_tcslen(status));
			} else {
				SetTextAlign(dc, TA_RIGHT);
				DrawTextUsingFont(dc, GlobalBoldFont, WIDTH - 26, 50, status, (int)_tcslen(status));
			}
			SetTextColor(dc, RGB(0, 0, 0));
		}

		return ENDPAINT;
	}

	case WM_KEYDOWN: {
		LocalMultiplayPlayer* player;
		LocalMultiplayPlayer* otherPlayer;
		int answer;
		switch (wParam) {
		case 'W':
		case 'A':
		case 'S':
		case 'D':
			if (g_Multiplay->Phase == 1 || g_Multiplay->Phase == -2) return 0;

			player = g_Multiplay->Players;
			otherPlayer = g_Multiplay->Players + 1;
			answer = wParam == 'W' ? 0 : (wParam == 'A' ? 1 : (wParam == 'S' ? 2 : 3));
			break;

		case VK_UP:
		case VK_LEFT:
		case VK_DOWN:
		case VK_RIGHT:
			if (g_Multiplay->Phase == -1 || g_Multiplay->Phase == 2) return 0;

			player = g_Multiplay->Players + 1;
			otherPlayer = g_Multiplay->Players;
			answer = wParam == VK_UP ? 0 : (wParam == VK_LEFT ? 1 : (wParam == VK_DOWN ? 2 : 3));
			break;

		default: return 0;
		}

		if (!IsWindowEnabled(g_Buttons[answer])) return 0;
		else if (answer != g_Question.Answer) {
			g_Multiplay->Phase += (g_Multiplay->Phase == 0 ? (player == g_Multiplay->Players ? 1 : -1) : (g_Multiplay->Phase > 0 ? 1 : -1));
			g_Winner = 0;
			if (abs(g_Multiplay->Phase) == 3) {
				--player->Score;
				++player->Wrong;

				g_Multiplay->Phase = 0;
				g_Winner = -2 + (player == g_Multiplay->Players);
			}

			InvalidateRect(handle, NULL, FALSE);
			EnableWindow(g_Buttons[answer], FALSE);
		} else {
			if (abs(g_Multiplay->Phase) <= 1) {
				player->Score += 2;
				++player->FirstCorrect;
			} else if (abs(g_Multiplay->Phase) == 2) {
				++player->Score;
				++player->SecondCorrect;
			}

			if (abs(g_Multiplay->Phase) > 0) {
				--otherPlayer->Score;
				++otherPlayer->Wrong;
			}

			g_Multiplay->Phase = 0;
			g_Winner = 1 + (player != g_Multiplay->Players);
		}

		if (g_Multiplay->Phase == 0) {
			ShowNextQuestion(handle);
		}
		return 0;
	}

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case 4: {
			TCHAR text[51] = _T("최종 점수\n왼쪽 플레이어: ");
			TCHAR temp[12];
			_itot(g_Multiplay->Players[0].Score, temp, 10);
			_tcscat(text, temp);
			_tcscat(text, _T("점\n오른쪽 플레이어: "));
			_itot(g_Multiplay->Players[1].Score, temp, 10);
			_tcscat(text, temp);
			_tcscat(text, _T("점"));

			MessageBox(handle, text, _T("정보"), MB_OK | MB_ICONINFORMATION);
			SendMessage(handle, WM_CLOSE, 0, 0);
			break;
		}
		}
		return 0;

	case WM_USER:
		g_QuestionOption = (QuestionOption*)lParam;
		ShowNextQuestion(handle);
		return 0;

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

void ShowNextQuestion(HWND handle) {
	GenerateQuestion(&g_Question, g_QuestionOption, NULL, 4, NULL);
	SetSelectorText(&g_Question, g_QuestionOption, g_Buttons, 4, false);
	InvalidateRect(handle, NULL, FALSE);
}