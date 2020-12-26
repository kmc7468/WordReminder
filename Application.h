#pragma once

#include "Multiplay.h"
#include "WinAPI.h"

#include <stdbool.h>

#define WR_APPLICATION_VERSION _T("1.4.0")

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