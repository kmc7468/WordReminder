#include "Window.h"

#include "Multiplay.h"
#include "Version.h"
#include "Word.h"

#include <stdlib.h>

static HWND g_SelectVocabularyButton;
static HWND g_GuessMeaningButton, g_GuessWordButton, g_GuessPronunciationButton;
static HWND g_GivePronunciationButton, g_ExcludeDuplicatedAnswer;
static HWND g_StartButton;

static LPCTSTR g_VocabularyPath;
static Vocabulary* g_Vocabularary;
static OnlineMultiplayOption* g_MultiplayOption;
static bool g_IsLocalMultiplay;

static bool g_ShouldEnableMainWindow = true;

LRESULT CALLBACK QuestionOptionWindowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam) {
	EVENT {
	case WM_CREATE:
		SetWindowLong(handle, GWL_STYLE, GetWindowLong(handle, GWL_STYLE) & ~WS_SIZEBOX & ~WS_MAXIMIZEBOX);
		SetWindowPos(handle, HWND_TOP, 0, 0, 500, 350, SWP_NOMOVE);

		g_SelectVocabularyButton = CreateAndShowChild(_T("button"), _T("단어장 선택하기"), GlobalBoldFont, BS_PUSHBUTTON,
			10, 35, 465, 50, handle, 0);

		g_GuessMeaningButton = CreateAndShowChild(_T("button"), _T("단어 보고 뜻 맞히기"), GlobalDefaultFont, BS_AUTOCHECKBOX,
			10, 130, 150, 15, handle, 1);
		g_GuessWordButton = CreateAndShowChild(_T("button"), _T("뜻 보고 단어 맞히기"), GlobalDefaultFont, BS_AUTOCHECKBOX,
			200, 130, 150, 15, handle, 2);
		g_GuessPronunciationButton = CreateAndShowChild(_T("button"), _T("단어 보고 발음 맞히기"), GlobalDefaultFont, BS_AUTOCHECKBOX,
			10, 155, 170, 15, handle, 3);
		SendMessage(g_GuessMeaningButton, BM_SETCHECK, !!Setting.GuessMeaning, 0);
		SendMessage(g_GuessWordButton, BM_SETCHECK, !!Setting.GuessWord, 0);
		SendMessage(g_GuessPronunciationButton, BM_SETCHECK, !!Setting.GuessPronunciation, 0);

		g_GivePronunciationButton = CreateAndShowChild(_T("button"), _T("발음 표시하기"), GlobalDefaultFont, BS_AUTOCHECKBOX,
			10, 215, 115, 15, handle, 4);
		g_ExcludeDuplicatedAnswer = CreateAndShowChild(_T("button"), _T("중복된 문제 제외하기"), GlobalDefaultFont, BS_AUTOCHECKBOX,
			145, 215, 160, 15, handle, 5);
		SendMessage(g_GivePronunciationButton, BM_SETCHECK, !!Setting.GivePronunciation, 0);
		SendMessage(g_ExcludeDuplicatedAnswer, BM_SETCHECK, !!Setting.ExcludeDuplicatedAnswer, 0);

		g_StartButton = CreateAndShowChild(_T("button"), _T("시작하기"), GlobalBoldFont, BS_PUSHBUTTON,
			10, 250, 465, 50, handle, 6);
		return 0;

	case WM_DESTROY:
		g_VocabularyPath = NULL;
		if (g_Vocabularary) {
			DestroyVocabulary(g_Vocabularary);
			free(g_Vocabularary);
			g_Vocabularary = NULL;
		}
		if (g_MultiplayOption) {
			free(g_MultiplayOption);
			g_MultiplayOption = NULL;
		}
		g_IsLocalMultiplay = false;

		if (g_ShouldEnableMainWindow) {
			EnableWindow(MainWindow, TRUE);
			SetWindowPos(MainWindow, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}
		g_ShouldEnableMainWindow = true;
		return 0;

	case WM_PAINT: {
		BEGINPAINT;

		DrawTextUsingFont(dc, GlobalBoldFont, 10, 10, STRING("단어장"));
		DrawTextUsingFont(dc, GlobalBoldFont, 10, 105, STRING("문제 유형"));
		DrawTextUsingFont(dc, GlobalBoldFont, 10, 190, STRING("기타 옵션"));

		return ENDPAINT;
	}

	case WM_CTLCOLORSTATIC: {
		const HWND btnHandle = (HWND)lParam;
		if (btnHandle == g_GuessMeaningButton || btnHandle == g_GuessWordButton ||
			btnHandle == g_GivePronunciationButton) {
			SetBkMode((HDC)wParam, TRANSPARENT);
		}
		return 0;
	}

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case 0: {
			const LPCTSTR path = ShowOpenFileDialog(handle);
			if (path) {
				SetWindowText(g_SelectVocabularyButton, _T("단어장 선택됨"));
				g_VocabularyPath = path;
				if (g_Vocabularary) {
					DestroyVocabulary(g_Vocabularary);
					free(g_Vocabularary);
					g_Vocabularary = NULL;
				}
			}
			break;
		}

		case 6: {
			const bool guessMeaning = (bool)IsDlgButtonChecked(handle, 1);
			const bool guessWord = (bool)IsDlgButtonChecked(handle, 2);
			const bool guessPronunciation = (bool)IsDlgButtonChecked(handle, 3);
			if (!g_VocabularyPath && !g_Vocabularary) {
				MessageBox(handle, _T("단어장을 선택해 주세요."), _T("오류"), MB_OK | MB_ICONERROR);
				break;
			} else if (!guessMeaning && !guessWord && !guessPronunciation) {
				MessageBox(handle, _T("문제 유형을 적어도 하나 선택해 주세요."), _T("오류"), MB_OK | MB_ICONERROR);
				break;
			}

			QuestionOption* option = calloc(1, sizeof(QuestionOption));
			if (g_Vocabularary) {
				option->Vocabulary = *g_Vocabularary;
			} else if (!LoadVocabulary(&option->Vocabulary, g_VocabularyPath)) {
				MessageBox(handle, _T("단어장을 읽는 중 오류가 발생했습니다. 올바른 단어장인지 확인해 보세요."), _T("오류"), MB_OK | MB_ICONERROR);
				free(option);
				break;
			}
			option->QuestionType |= guessMeaning ? GuessMeaning : 0;
			option->QuestionType |= guessWord ? GuessWord : 0;
			option->QuestionType |= guessPronunciation ? GuessPronunciation : 0;
			option->GivePronunciation = (bool)IsDlgButtonChecked(handle, 4);
			if (!g_MultiplayOption && !g_IsLocalMultiplay) {
				option->ExcludeDuplicatedAnswer = (bool)IsDlgButtonChecked(handle, 5);
			}

			if (guessPronunciation) {
				int unique = option->Vocabulary.Count;
				for (int i = 0; i < option->Vocabulary.Count; ++i) {
					if (option->Vocabulary.Array[i].Pronunciation[0] == 0 ||
						_tcscmp(option->Vocabulary.Array[i].Word, option->Vocabulary.Array[i].Pronunciation) == 0) {
						MessageBox(handle, _T("단어장에 있는 모든 단어가 발음 데이터를 갖고 있지 않아 단어와 뜻 보고 단어 맞추기 유형은 사용할 수 없습니다."), _T("오류"), MB_OK | MB_ICONERROR);
						if (!g_Vocabularary) {
							DestroyVocabulary(&option->Vocabulary);
						}
						free(option);
						return 0;
					}

					for (int j = 0; j < i; ++j) {
						if (_tcscmp(option->Vocabulary.Array[i].Pronunciation, option->Vocabulary.Array[j].Pronunciation) == 0) {
							--unique;
							break;
						}
					}
				}

				if (unique < 5) {
					MessageBox(handle, _T("단어장에 있는 단어의 서로 다른 발음 데이터의 개수가 5개 미만이기 때문에 단어와 뜻 보고 단어 맞추기 유형은 사용할 수 없습니다."), _T("오류"), MB_OK | MB_ICONERROR);
					if (!g_Vocabularary) {
						DestroyVocabulary(&option->Vocabulary);
					}
					free(option);
					break;
				}
			}

			if (g_IsLocalMultiplay) {
				const HWND localMultiplayWindow = CreateAndShowWindow(_T("LocalMultiplayWindow"), _T("로컬 멀티 플레이"), SW_SHOW);
				SendMessage(localMultiplayWindow, WM_USER, 0, (LPARAM)option);
			} else if (!g_MultiplayOption || g_MultiplayOption->Mode == TurnMode || g_MultiplayOption->Role == Examinee) {
				const HWND questionWindow = CreateAndShowWindow(_T("QuestionWindow"), _T("단어 암기하기"), SW_SHOW);
				SendMessage(questionWindow, WM_USER, 0, (LPARAM)option);
				if (g_MultiplayOption) {
					SetWindowText(questionWindow, _T("온라인 멀티 플레이"));
					SendMessage(questionWindow, WM_USER + 1, 0, (LPARAM)g_MultiplayOption);
				}
			} else {
				const HWND examinerWindow = CreateAndShowWindow(_T("ExaminerWindow"), _T("온라인 멀티 플레이"), SW_SHOW);
				SendMessage(examinerWindow, WM_USER, 0, (LPARAM)option);
				SendMessage(examinerWindow, WM_USER + 1, 0, (LPARAM)g_MultiplayOption);
			}

			if (g_Vocabularary) {
				free(g_Vocabularary);
				g_Vocabularary = NULL;
			}
			g_MultiplayOption = NULL;

			Setting.GuessMeaning = (bool)IsDlgButtonChecked(handle, 1);
			Setting.GuessWord = (bool)IsDlgButtonChecked(handle, 2);
			Setting.GuessPronunciation = (bool)IsDlgButtonChecked(handle, 3);
			Setting.GivePronunciation = (bool)IsDlgButtonChecked(handle, 4);
			if (!g_MultiplayOption && !g_IsLocalMultiplay) {
				Setting.ExcludeDuplicatedAnswer = (bool)IsDlgButtonChecked(handle, 5);
			}

			g_ShouldEnableMainWindow = false;
			SendMessage(handle, WM_CLOSE, 0, 0);
			break;
		}
		}
		return 0;

	case WM_USER:
		g_Vocabularary = (Vocabulary*)lParam;
		for (int i = 0; i < g_Vocabularary->Count; ++i) {
			g_Vocabularary->Array[i].IsWrong = false;
		}
		SetWindowText(g_SelectVocabularyButton, _T("단어장 선택됨"));
		return 0;

	case WM_USER + 1:
		g_MultiplayOption = (OnlineMultiplayOption*)lParam;
		SetWindowText(g_StartButton, _T("서버 만들기"));

		ShowWindow(g_ExcludeDuplicatedAnswer, SW_HIDE);
		return 0;

	case WM_USER + 2:
		g_IsLocalMultiplay = true;

		ShowWindow(g_ExcludeDuplicatedAnswer, SW_HIDE);
		return 0;

	case WM_CLOSE:
		DestroyWindow(handle);
		return 0;
	}
	return DefWindowProc(handle, message, wParam, lParam);
}