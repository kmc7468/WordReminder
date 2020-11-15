#pragma once

#include "Version.h"
#include "Word.h"

#include <stdbool.h>
#include <WinSock2.h>
#include <Windows.h>

typedef enum {
	Server,
	Client,
} SocketType;

typedef enum {
	TurnMode, // 턴제 모드
	FixedMode, // 역할 고정 모드
} MultiplayMode;

typedef enum {
	Examiner, // 출제자
	Examinee, // 응시자
} MultiplayRole;

typedef struct {
	LPSTR ServerIp;
	int ServerPort;
	SocketType SocketType;
	MultiplayMode Mode;
	MultiplayRole Role;
} MultiplayOption;

typedef struct {
	SOCKET Socket;
	SOCKADDR_IN Address;
	int Correct;
	int Wrong;
} MultiplayPlayer;

typedef enum {
	Singleplay,
	OpeningServer,
	WaitingForPlayer,
	PlayerJoining,
	JoiningServer,
	Connected,
	SentAnswer,
} MultiplayStatus;

typedef struct {
	MultiplayOption* Option;
	MultiplayPlayer Players[2];
	MultiplayStatus Status;

	HWND Window;
	Question* Question;
	QuestionOption* QuestionOption;
} Multiplay;

bool OpenServer(Multiplay* multiplay, MultiplayOption* multiplayOption);
bool WaitForPlayer(Multiplay* multiplay);
bool JoinServer(Multiplay* multiplay, MultiplayOption* multiplayOption);
void DestroyMultiplay(Multiplay* multiplay);

bool Send(Multiplay* multiplay, const void* data, int length);
bool Receive(Multiplay* multiplay, void* buffer, int length);
bool SendVersion(Multiplay* multiplay);
bool ReceiveVersion(Multiplay* multiplay, LPTSTR* serverVersion, int* protocolVersion);
bool SendBool(Multiplay* multiplay, bool data);
bool ReceiveBool(Multiplay* multiplay, bool* buffer);
bool SendInt(Multiplay* multiplay, int data);
bool ReceiveInt(Multiplay* multiplay, int* buffer);
bool SendString(Multiplay* multiplay, LPCTSTR data);
bool ReceiveString(Multiplay* multiplay, LPTSTR* buffer);
bool SendVocabulary(Multiplay* multiplay);
bool ReceiveVocabulary(Multiplay* multiplay);

bool SendHttpRequest(LPCSTR address, LPCSTR request, int requestLength, LPSTR response, int responseLength);

void StartMultiplay(Multiplay* multiplay, MultiplayOption* option, Question* question, QuestionOption* questionOption, HWND handle);
void StopMultiplay(Multiplay* multiplay);
void SendQuestion(Multiplay* multiplay, HWND* buttons, int answer);
void SendAnswer(Multiplay* multiplay);
void RequestChangeRole(Multiplay* multiplay);