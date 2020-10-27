#include "Window.h"

#include "Word.h"

#include <stdlib.h>

static QuestionOption* g_QuestionOption;
static Question g_Question;
static bool g_IsWrong;

static HFONT g_QuestionFont, g_WordOrMeaningFont, g_PronunciationFont, g_ButtonFont;
static HWND g_Buttons[5];

static void ShowNextQuestion(HWND handle);

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
		return 0;

	case WM_DESTROY:
		DestroyVocabulary(&g_QuestionOption->Vocabulary);
		free(g_QuestionOption);

		DeleteObject(g_QuestionFont);
		DeleteObject(g_WordOrMeaningFont);
		DeleteObject(g_PronunciationFont);
		DeleteObject(g_ButtonFont);
		EnableWindow(MainWindow, TRUE);
		return 0;

	case WM_SIZE:
		DeleteObject(g_ButtonFont);
		g_ButtonFont = CreateGlobalFont(18 * HEIGHT / 480, false);
		for (int i = 0; i < 5; ++i) {
			SetWindowPos(g_Buttons[i], HWND_TOP, WIDTH / 2 - WIDTH / 4, 140 + ((HEIGHT / 10 + HEIGHT / 50) * i), WIDTH / 2, HEIGHT / 10, 0);
			SendMessage(g_Buttons[i], WM_SETFONT, (WPARAM)g_ButtonFont, true);
		}
		return 0;

	case WM_PAINT: {
		PAINTSTRUCT ps;
		const HDC dc = BeginPaint(handle, &ps);
		SetTextAlign(dc, TA_CENTER);

		const Word* const answer = g_Question.Words[g_Question.Answer];
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

		EndPaint(handle, &ps);
		return 0;
	}

	case WM_COMMAND:
		if (LOWORD(wParam) == g_Question.Answer) {
			g_IsWrong = false;
			ShowNextQuestion(handle);
		} else {
			g_IsWrong = true;
			EnableWindow(g_Buttons[LOWORD(wParam)], FALSE);
			InvalidateRect(handle, NULL, TRUE);
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
	GenerateQuestion(&g_Question, g_QuestionOption);

	for (int i = 0; i < 5; ++i) {
		if (g_Question.Type == GuessingMeaning) {
			SetWindowText(g_Buttons[i], g_Question.Words[i]->Meaning);
		} else {
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
		}
		EnableWindow(g_Buttons[i], TRUE);
	}
	InvalidateRect(handle, NULL, TRUE);
}