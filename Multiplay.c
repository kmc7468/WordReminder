#include "Multiplay.h"

#include <stdlib.h>
#include <string.h>
#include <tchar.h>

#define My multiplay->Players[0]
#define Other multiplay->Players[1]

bool OpenServer(Multiplay* multiplay, MultiplayOption* multiplayOption) {
	multiplay->Option = multiplayOption;
	memset(multiplay->Players, 0, sizeof(multiplay->Players));

	if ((My.Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == SOCKET_ERROR) return false;

	My.Address.sin_addr.s_addr = INADDR_ANY;
	My.Address.sin_family = AF_INET;
	My.Address.sin_port = htons((u_short)multiplayOption->ServerPort);
	if (bind(My.Socket, (SOCKADDR*)&My.Address, sizeof(My.Address))) {
		closesocket(My.Socket);
		return false;
	} else return true;
}
bool WaitForPlayer(Multiplay* multiplay) {
	if (listen(My.Socket, 1) == SOCKET_ERROR) return false;
	else return (Other.Socket = accept(My.Socket, (SOCKADDR*)&Other.Address, NULL)) != SOCKET_ERROR;
}
bool JoinServer(Multiplay* multiplay, MultiplayOption* multiplayOption) {
	multiplay->Option = multiplayOption;
	memset(multiplay->Players, 0, sizeof(multiplay->Players));

	if ((My.Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == SOCKET_ERROR) return false;

	const PHOSTENT host = gethostbyname(multiplayOption->ServerIp);

	Other.Address.sin_addr = *(IN_ADDR*)host->h_addr_list[0];
	Other.Address.sin_family = AF_INET;
	Other.Address.sin_port = htons((u_short)multiplayOption->ServerPort);
	if (connect(My.Socket, (SOCKADDR*)&Other.Address, sizeof(Other.Address))) {
		closesocket(My.Socket);
		return false;
	} else return true;
}
void FinishMultiplay(Multiplay* multiplay) {
	closesocket(My.Socket);
}
void DestroyMultiplay(Multiplay* multiplay) {
	FinishMultiplay(multiplay);
	free(multiplay->Option->ServerIp);
	free(multiplay->Option);
}

bool Send(Multiplay* multiplay, const void* data, int length) {
	const char* current = data;
	int sent;
	while ((sent = send(Other.Socket, current, (int)(length - (current - (const char*)data)), 0)) > 0) {
		current += sent;
	}
	return sent != SOCKET_ERROR;
}
bool Receive(Multiplay* multiplay, void* buffer, int length) {
	char* current = buffer;
	int received;
	while ((received = recv(Other.Socket, current, (int)(length - (current - (char*)buffer)), 0)) > 0) {
		current += received;
	}
	return received != SOCKET_ERROR;
}
bool SendInt(Multiplay* multiplay, int data) {
	return Send(multiplay, &data, sizeof(data));
}
bool ReceiveInt(Multiplay* multiplay, int* buffer) {
	return Receive(multiplay, buffer, sizeof(*buffer));
}
bool SendString(Multiplay* multiplay, LPCTSTR data) {
	const LPCWSTR raw = GetRawString(data);
	const int rawLength = (int)wcslen(raw);
	if (!SendInt(multiplay, rawLength)) {
		FreeRawString(raw);
		return false;
	}

	const bool result = Send(multiplay, raw, sizeof(WCHAR) * rawLength);
	return FreeRawString(raw), result;
}
bool ReceiveString(Multiplay* multiplay, LPTSTR* buffer) {
	int rawLength;
	if (!ReceiveInt(multiplay, &rawLength)) return false;
	const LPWSTR raw = malloc(sizeof(WCHAR) * rawLength);
	if (Receive(multiplay, raw, sizeof(WCHAR) * rawLength)) {
		*buffer = MakeUniString(raw);
		return true;
	} else {
		free(raw);
		return false;
	}
}
bool SendVocabulary(Multiplay* multiplay) {
	if (!SendInt(multiplay, multiplay->Option->Vocabulary->Count)) return false;
	for (int i = 0; i < multiplay->Option->Vocabulary->Count; ++i) {
		if (!SendString(multiplay, multiplay->Option->Vocabulary->Array[i].Word)) return false;
		if (!SendString(multiplay, multiplay->Option->Vocabulary->Array[i].Pronunciation)) return false;
		if (!SendString(multiplay, multiplay->Option->Vocabulary->Array[i].Meaning)) return false;
	}
	return true;
}
bool ReceiveVocabulary(Multiplay* multiplay) {
	int count;
	if (!ReceiveInt(multiplay, &count)) return false;
	for (int i = 0; i < count; ++i) {
		Word word = { NULL, NULL, NULL };
		bool success = true;
		success = success && ReceiveString(multiplay, &word.Word);
		success = success && ReceiveString(multiplay, &word.Pronunciation);
		success = success && ReceiveString(multiplay, &word.Meaning);
		if (success) {
			AddWord(multiplay->Option->Vocabulary, &word);
		} else {
			free(word.Word);
			free(word.Pronunciation);
			free(word.Meaning);
			DestroyVocabulary(multiplay->Option->Vocabulary);
			return false;
		}
	}
	return true;
}