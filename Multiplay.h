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
	TurnMode, // 턴제 모드
	FixedMode, // 역할 고정 모드
} OnlineMultiplayMode;

typedef enum {
	Examiner, // 출제자
	Examinee, // 응시자
} OnlineMultiplayRole;

typedef struct {
	LPSTR ServerIp;
	int ServerPort;
	SocketType SocketType;
	OnlineMultiplayMode Mode;
	OnlineMultiplayRole Role;
} OnlineMultiplayOption;

typedef struct {
	SOCKET Socket;
	SOCKADDR_IN Address;
	int Correct;
	int Wrong;
} OnlineMultiplayPlayer;

typedef enum {
	Singleplay,
	OpeningServer,
	WaitingForPlayer,
	PlayerJoining,
	JoiningServer,
	Connected,
	SentAnswer,
} OnlineMultiplayStatus;

typedef struct {
	OnlineMultiplayOption* Option;
	OnlineMultiplayPlayer Players[2];
	OnlineMultiplayStatus Status;

	HWND Window;
	Question* Question;
	QuestionOption* QuestionOption;
} OnlineMultiplay;

bool OpenServer(OnlineMultiplay* multiplay, OnlineMultiplayOption* multiplayOption);
bool WaitForPlayer(OnlineMultiplay* multiplay);
bool JoinServer(OnlineMultiplay* multiplay, OnlineMultiplayOption* multiplayOption);
void DestroyOnlineMultiplay(OnlineMultiplay* multiplay);

bool Send(OnlineMultiplay* multiplay, const void* data, int length);
bool Receive(OnlineMultiplay* multiplay, void* buffer, int length);
bool SendVersion(OnlineMultiplay* multiplay);
bool ReceiveVersion(OnlineMultiplay* multiplay, LPTSTR* serverVersion, int* protocolVersion);
bool SendBool(OnlineMultiplay* multiplay, bool data);
bool ReceiveBool(OnlineMultiplay* multiplay, bool* buffer);
bool SendInt(OnlineMultiplay* multiplay, int data);
bool ReceiveInt(OnlineMultiplay* multiplay, int* buffer);
bool SendString(OnlineMultiplay* multiplay, LPCTSTR data);
bool ReceiveString(OnlineMultiplay* multiplay, LPTSTR* buffer);
bool SendVocabulary(OnlineMultiplay* multiplay);
bool ReceiveVocabulary(OnlineMultiplay* multiplay);

bool SendHttpRequest(LPCSTR address, LPCSTR request, int requestLength, LPSTR response, int responseLength);

void StartOnlineMultiplay(OnlineMultiplay* multiplay, OnlineMultiplayOption* option, Question* question, QuestionOption* questionOption, HWND handle);
void StopOnlineMultiplay(OnlineMultiplay* multiplay);
void SendQuestion(OnlineMultiplay* multiplay, HWND* buttons, int answer);
void SendAnswer(OnlineMultiplay* multiplay);
void RequestChangeRole(OnlineMultiplay* multiplay);