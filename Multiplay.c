#include "Multiplay.h"

#include "Application.h"
#include "String.h"

#include <stdlib.h>
#include <string.h>

bool OpenServer(OnlineMultiplay* onlineMultiplay, OnlineMultiplayOption* onlineMultiplayOption) {
	onlineMultiplay->Option = onlineMultiplayOption;
	CreateArray(&onlineMultiplay->Players, sizeof(OnlineMultiplayPlayer*));

	OnlineMultiplayPlayer* const me = calloc(1, sizeof(OnlineMultiplayPlayer));
	me->Id = onlineMultiplay->UsableId++;

	onlineMultiplay->Me = me;
	onlineMultiplay->Server = me;
	AddElement(&onlineMultiplay->Players, &me);

	if ((me->Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == SOCKET_ERROR) goto Destroy;

	me->Address.sin_addr.s_addr = INADDR_ANY;
	me->Address.sin_family = AF_INET;
	me->Address.sin_port = htons((u_short)onlineMultiplayOption->ServerPort);
	if (bind(me->Socket, (SOCKADDR*)&me->Address, sizeof(me->Address)) == 0) return true;

	closesocket(me->Socket);

Destroy:
	free(me);
	DestroyArray(&onlineMultiplay->Players);
	return false;
}
bool JoinServer(OnlineMultiplay* onlineMultiplay, OnlineMultiplayOption* onlineMultiplayOption) {
	onlineMultiplay->Option = onlineMultiplayOption;
	CreateArray(&onlineMultiplay->Players, sizeof(OnlineMultiplayPlayer*));

	OnlineMultiplayPlayer* const me = calloc(1, sizeof(OnlineMultiplayPlayer));
	OnlineMultiplayPlayer* const server = calloc(1, sizeof(OnlineMultiplayPlayer));

	onlineMultiplay->Me = me;
	onlineMultiplay->Server = server;
	AddElement(&onlineMultiplay->Players, &me);
	AddElement(&onlineMultiplay->Players, &server);

	if ((me->Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == SOCKET_ERROR) goto DestroyWithoutSocket;

	const PHOSTENT host = gethostbyname(onlineMultiplayOption->ServerAddress);
	if (!host) goto Destroy;

	server->Address.sin_addr.s_addr = *(ULONG*)host->h_addr;
	server->Address.sin_family = AF_INET;
	server->Address.sin_port = htons((u_short)onlineMultiplayOption->ServerPort);
	if (connect(me->Socket, (SOCKADDR*)&server->Address, sizeof(server->Address)) == 0) return true;

Destroy:
	closesocket(me->Socket);

DestroyWithoutSocket:
	free(me);
	DestroyArray(&onlineMultiplay->Players);
	return false;
}
void DestroyOnlineMultiplay(OnlineMultiplay* onlineMultiplay) {
	free(onlineMultiplay->Option->ServerAddress);
	free(onlineMultiplay->Option);

	for (int i = 0; i < onlineMultiplay->Players.Count; ++i) {
		OnlineMultiplayPlayer* const player = *(OnlineMultiplayPlayer**)GetElement(&onlineMultiplay->Players, i);
		closesocket(player->Socket);
		free(player);
	}
	DestroyArray(&onlineMultiplay->Players);
}

bool Send(OnlineMultiplayPlayer* onlineMultiplayPlayer, const void* data, int length) {
	int sum = 0;
	do {
		const int sent = send(onlineMultiplayPlayer->Socket, (char*)data + sum, length - sum, 0);
		if (sent == SOCKET_ERROR) return false;

		sum += sent;
	} while (sum < length);
	return true;
}
bool Receive(OnlineMultiplayPlayer* onlineMultiplayPlayer, void* buffer, int length) {
	int sum = 0;
	do {
		const int received = recv(onlineMultiplayPlayer->Socket, (char*)buffer + sum, length - sum, 0);
		if (received == SOCKET_ERROR) return false;

		sum += received;
	} while (sum < length);
	return true;
}
bool SendBool(OnlineMultiplayPlayer* onlineMultiplayPlayer, bool data) {
	return Send(onlineMultiplayPlayer, &data, sizeof(data));
}
bool ReceiveBool(OnlineMultiplayPlayer* onlineMultiplayPlayer, bool* buffer) {
	return Receive(onlineMultiplayPlayer, buffer, sizeof(*buffer));
}
bool SendInt(OnlineMultiplayPlayer* onlineMultiplayPlayer, int data) {
	return Send(onlineMultiplayPlayer, &data, sizeof(data));
}
bool ReceiveInt(OnlineMultiplayPlayer* onlineMultiplayPlayer, int* buffer) {
	return Receive(onlineMultiplayPlayer, buffer, sizeof(*buffer));
}
bool SendString(OnlineMultiplayPlayer* onlineMultiplayPlayer, LPCTSTR data) {
	const LPCWSTR rawData = GetRawString(data);
	const int rawDataLength = (int)wcslen(rawData);

	const bool success = SendInt(onlineMultiplayPlayer, rawDataLength) &&
		Send(onlineMultiplayPlayer, rawData, sizeof(WCHAR) * rawDataLength);
	FreeRawString(rawData);
	return success;
}
bool ReceiveString(OnlineMultiplayPlayer* onlineMultiplayPlayer, LPTSTR* buffer) {
	int rawBufferLength;
	if (!ReceiveInt(onlineMultiplayPlayer, &rawBufferLength)) return false;

	const LPWSTR rawBuffer = calloc(rawBufferLength + 1, sizeof(WCHAR));
	if (Receive(onlineMultiplayPlayer, rawBuffer, sizeof(WCHAR) * rawBufferLength)) {
		*buffer = MakeGenericString(rawBuffer);
		return true;
	} else {
		free(rawBuffer);
		return false;
	}
}
bool SendVersion(OnlineMultiplayPlayer* onlineMultiplayPlayer) {
	return SendString(onlineMultiplayPlayer, WR_APPLICATION_VERSION) &&
		SendInt(onlineMultiplayPlayer, WR_ONLINE_MULTIPLAY_PROTOCOL_VERSION);
}
bool ReceiveVersion(OnlineMultiplayPlayer* onlineMultiplayPlayer, LPTSTR* serverVersionBuffer, int* protocolVersionBuffer) {
	return ReceiveString(onlineMultiplayPlayer, serverVersionBuffer) &&
		ReceiveInt(onlineMultiplayPlayer, protocolVersionBuffer);
}
bool SendVocabulary(OnlineMultiplayPlayer* onlineMultiplayPlayer, const Vocabulary* data) {
	if (!SendInt(onlineMultiplayPlayer, data->Words.Count)) return false;

	for (int i = 0; i < data->Words.Count; ++i) {
		Word* const word = GetWord((Vocabulary*)data, i);
		if (!SendString(onlineMultiplayPlayer, word->Word) ||
			!SendInt(onlineMultiplayPlayer, word->Meanings.Count)) return false;

		for (int j = 0; j < word->Meanings.Count; ++j) {
			Meaning* const meaning = GetMeaning(word, j);
			if (!SendString(onlineMultiplayPlayer, meaning->Pronunciation) ||
				!SendString(onlineMultiplayPlayer, meaning->Meaning)) return false;
		}
	}
	return true;
}
bool ReceiveVocabulary(OnlineMultiplayPlayer* onlineMultiplayPlayer, Vocabulary* buffer) {
	int wordCount;
	if (!ReceiveInt(onlineMultiplayPlayer, &wordCount)) return false;

	CreateVocabulary(buffer);

	for (int i = 0; i < wordCount; ++i) {
		Word word = { 0 };
		CreateWord(&word);

		int meaningCount;
		if (!ReceiveString(onlineMultiplayPlayer, &word.Word) ||
			!ReceiveInt(onlineMultiplayPlayer, &meaningCount)) goto Destroy;

		for (int j = 0; j < meaningCount; ++j) {
			Meaning meaning = { 0 };
			if (ReceiveString(onlineMultiplayPlayer, &meaning.Pronunciation) &&
				ReceiveString(onlineMultiplayPlayer, &meaning.Meaning)) {
				AddMeaning(&word, &meaning);
			} else {
				DestroyMeaning(&meaning);
				goto Destroy;
			}
		}
		continue;

	Destroy:
		DestroyWord(&word);
		DestroyVocabulary(buffer, true);
		return false;
	}
	return true;
}