#pragma once

#include "Word.h"

#include <WinSock2.h>
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
	LPSTR ServerIp;
	int ServerPort;
	SocketType SocketType;
	MultiplayMode Mode;
	MultiplayRole Role;
	Vocabulary* Vocabulary;
} MultiplayOption;

typedef struct {
	SOCKET Socket;
	int Correct;
	int Wrong;
} MultiplayPlayer;

typedef struct {
	MultiplayOption* Option;
	SOCKADDR_IN ServerAddress;
	MultiplayPlayer Players[2];
} Multiplay;

bool OpenServer(Multiplay* multiplay, MultiplayOption* multiplayOption);
bool JoinServer(Multiplay* multiplay, MultiplayOption* multiplayOption);
void FinishMultiplay(Multiplay* multiplay);
void DestroyMultiplay(Multiplay* multiplay);