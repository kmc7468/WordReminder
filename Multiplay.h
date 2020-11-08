#pragma once

#include "Word.h"

#include <stdbool.h>
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
	SOCKADDR_IN Address;
	int Correct;
	int Wrong;
} MultiplayPlayer;

typedef struct {
	MultiplayOption* Option;
	MultiplayPlayer Players[2];
} Multiplay;

bool OpenServer(Multiplay* multiplay, MultiplayOption* multiplayOption);
bool WaitForPlayer(Multiplay* multiplay);
bool JoinServer(Multiplay* multiplay, MultiplayOption* multiplayOption);
void FinishMultiplay(Multiplay* multiplay);
void DestroyMultiplay(Multiplay* multiplay);

bool Send(Multiplay* multiplay, const void* data, int length);
bool Receive(Multiplay* multiplay, void* buffer, int length);
bool SendInt(Multiplay* multiplay, int data);
bool ReceiveInt(Multiplay* multiplay, int* buffer);
bool SendString(Multiplay* multiplay, LPCTSTR data);
bool ReceiveString(Multiplay* multiplay, LPTSTR* buffer);
bool SendVocabulary(Multiplay* multiplay);
bool ReceiveVocabulary(Multiplay* multiplay);