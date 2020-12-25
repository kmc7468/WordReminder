#pragma once

#include "Multiplay.h"

#include <stdbool.h>
#include <tchar.h>
#include <WinSock2.h>
#include <Windows.h>

#define WR_APPLICATION_VERSION _T("1.3.0")
#define WR_ONLINE_MULTIPLAY_PROTOCOL_VERSION 1

typedef struct {
	// QuestionOptionWindow
	bool GuessMeaning;
	bool GuessWord;
	bool GuessPronunciation;
	bool GivePronunciation;
	bool ExcludeDuplicatedAnswer;

	// OnlineMultiplayWindow(Create)
	int NewServerPort;
	OnlineMultiplayMode NewServerMode;
	OnlineMultiplayRole NewServerRole;

	// OnlineMultiplayWindow(Join)
	LPTSTR ServerIp;
	int ServerPort;
} RegistryData;

extern RegistryData Setting;

void LoadSetting();
void SaveSetting();