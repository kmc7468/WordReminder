#pragma once

#include "Word.h"

#include <Windows.h>

typedef enum {
	Server,
	Client,
} SocketType;

typedef enum {
	TurnMode,
	FixedMode,
} MultiplayMode;

typedef enum {
	Examiner,
	Examinee,
} MultiplayRole;

typedef struct {
	LPTSTR ServerIp;
	int ServerPort;
	SocketType SocketType;
	MultiplayMode Mode;
	MultiplayRole Role;
	Vocabulary* Vocabulary;
} MultiplayOption;