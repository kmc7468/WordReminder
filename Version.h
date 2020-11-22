#pragma once

#include "Multiplay.h"

#include <stdbool.h>
#include <tchar.h>
#include <WinSock2.h>
#include <Windows.h>

#define WR_APPLICATION_VERSION _T("1.2.0")
#define WR_MULTIPLAY_PROTOCOL_VERSION 0

typedef struct {
	// QuestionOptionWindow
	bool GuessMeaning;
	bool GuessWord;
	bool GivePronunciation;

	// OnlineMultiplayWindow(Create)
	int NewServerPort;
	MultiplayMode NewServerMode;
	MultiplayRole NewServerRole;

	// OnlineMultiplayWindow(Join)
	LPTSTR ServerIp;
	int ServerPort;
} RegistryData;

extern RegistryData Setting;

void LoadSetting();
void SaveSetting();