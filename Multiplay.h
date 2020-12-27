#pragma once

#include "WinAPI.h"
#include "Word.h"

#include <stdbool.h>

#define WR_ONLINE_MULTIPLAY_PROTOCOL_VERSION 2

typedef enum {
	Server,
	Client,
} SocketType;

typedef enum {
	TurnBasedMode, // 턴제 모드
	FixedRoleMode, // 역할 고정 모드
} OnlineMultiplayMode;

typedef enum {
	Examiner, // 출제자
	Examinee, // 응시자
} OnlineMultiplayRole;

typedef struct {
	LPSTR ServerAddress;
	int ServerPort;
	int MaximumPlayers;

	OnlineMultiplayMode Mode;
	OnlineMultiplayRole Role;

	SocketType SocketType;
} OnlineMultiplayOption;

typedef enum {
	Opening = 1,
	Joining,
	Waiting,
	Playing,
} OnlineMultiplayStatus;

typedef struct {
	SOCKET Socket;
	SOCKADDR_IN Address;
	int Id;
	LPTSTR Username;
	OnlineMultiplayStatus Status;
} OnlineMultiplayPlayer;

typedef struct {
	OnlineMultiplayOption* Option;
	Array Players;
	int UsableId;
	OnlineMultiplayPlayer* Me;
	OnlineMultiplayPlayer* Server;

	HWND WaitingScene;
	Question* Question;
} OnlineMultiplay;

bool OpenServer(OnlineMultiplay* onlineMultiplay, OnlineMultiplayOption* onlineMultiplayOption);
bool JoinServer(OnlineMultiplay* onlineMultiplay, OnlineMultiplayOption* onlineMultiplayOption);
void DestroyOnlineMultiplay(OnlineMultiplay* onlineMultiplay);

bool Send(OnlineMultiplayPlayer* onlineMultiplayPlayer, const void* data, int length);
bool Receive(OnlineMultiplayPlayer* onlineMultiplayPlayer, void* buffer, int length);
bool SendBool(OnlineMultiplayPlayer* onlineMultiplayPlayer, bool data);
bool ReceiveBool(OnlineMultiplayPlayer* onlineMultiplayPlayer, bool* buffer);
bool SendInt(OnlineMultiplayPlayer* onlineMultiplayPlayer, int data);
bool ReceiveInt(OnlineMultiplayPlayer* onlineMultiplayPlayer, int* buffer);
bool SendString(OnlineMultiplayPlayer* onlineMultiplayPlayer, LPCTSTR data);
bool ReceiveString(OnlineMultiplayPlayer* onlineMultiplayPlayer, LPTSTR* buffer);
bool SendVersion(OnlineMultiplayPlayer* onlineMultiplayPlayer);
bool ReceiveVersion(OnlineMultiplayPlayer* onlineMultiplayPlayer, LPTSTR* serverVersionBuffer, int* protocolVersionBuffer);
bool SendVocabulary(OnlineMultiplayPlayer* onlineMultiplayPlayer, const Vocabulary* data);
bool ReceiveVocabulary(OnlineMultiplayPlayer* onlineMultiplayPlayer, Vocabulary* buffer);

typedef struct {
	int Score;
} LocalMultiplayPlayer;

typedef struct {
	LocalMultiplayPlayer Players[2];
	int Phase;
} LocalMultiplay;