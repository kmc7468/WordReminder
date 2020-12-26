#pragma once

#include "Multiplay.h"
#include "WinAPI.h"

#include <stdbool.h>

#define WR_APPLICATION_VERSION _T("1.4.0")

extern HINSTANCE Instance;

bool InitializeApplication(HINSTANCE instance);
void DestroyApplication();

int GetAppropriateLengthForDpi(HWND window, int originalLength);
int GetAppropriateLengthForSize(HWND window, int originalLength);

HFONT CreateGlobalFont(int height, bool isBold);

LPCTSTR ShowOpenFileDialog(HWND window);
LPCTSTR ShowSaveFileDialog(HWND window);

typedef struct {
	// QuestionOptionScene
	bool GuessMeaning;
	bool GuessWord;
	bool GuessPronunciation;

	bool ShowPronunciation;
	bool ExcludeDuplicatedAnswer;

	// OnlineMultiplayScene
	int NewServerPort;
	OnlineMultiplayMode NewServerMode;
	OnlineMultiplayRole NewServerRole;

	LPTSTR ServerIp;
	int ServerPort;
} RegistryData;

extern RegistryData Setting;

void LoadSetting();
void SaveSetting();