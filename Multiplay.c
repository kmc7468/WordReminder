#include "Multiplay.h"

#include <stdlib.h>
#include <string.h>
#include <tchar.h>

#define My multiplay->Players[0]
#define Other multiplay->Players[1]

static SOCKET GetClientSocket(Multiplay* multiplay) {
	return multiplay->Option->SocketType == Server ? Other.Socket : My.Socket;
}

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
	if (!host) return false;

	Other.Address.sin_addr.s_addr = *(ULONG*)host->h_addr;
	Other.Address.sin_family = AF_INET;
	Other.Address.sin_port = htons((u_short)multiplayOption->ServerPort);
	if (connect(My.Socket, (SOCKADDR*)&Other.Address, sizeof(Other.Address))) {
		closesocket(My.Socket);
		return false;
	} else return true;
}
void DestroyMultiplay(Multiplay* multiplay) {
	closesocket(My.Socket);
	free(multiplay->Option->ServerIp);
	free(multiplay->Option);
}

bool Send(Multiplay* multiplay, const void* data, int length) {
	int sent = 0;
	do {
		const int result = send(GetClientSocket(multiplay), (const char*)data + sent, length - sent, 0);
		if (result == SOCKET_ERROR) return false;
		sent += result;
	} while (sent < length);
	return true;
}
bool Receive(Multiplay* multiplay, void* buffer, int length) {
	int received = 0;
	do {
		const int result = recv(GetClientSocket(multiplay), (char*)buffer + received, length - received, 0);
		if (result == SOCKET_ERROR) return false;
		received += result;
	} while (received < length);
	return true;
}
bool SendVersion(Multiplay* multiplay) {
	return SendString(multiplay, WR_APPLICATION_VERSION) && SendInt(multiplay, WR_MULTIPLAY_PROTOCOL_VERSION);
}
bool ReceiveVersion(Multiplay* multiplay, LPTSTR* serverVersion, int* protocolVersion) {
	return ReceiveString(multiplay, serverVersion) && ReceiveInt(multiplay, protocolVersion);
}
bool SendBool(Multiplay* multiplay, bool data) {
	return Send(multiplay, &data, sizeof(data));
}
bool ReceiveBool(Multiplay* multiplay, bool* buffer) {
	return Receive(multiplay, buffer, sizeof(*buffer));
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
	const bool result = SendInt(multiplay, rawLength) && Send(multiplay, raw, sizeof(WCHAR) * rawLength);
	return FreeRawString(raw), result;
}
bool ReceiveString(Multiplay* multiplay, LPTSTR* buffer) {
	int rawLength;
	if (!ReceiveInt(multiplay, &rawLength)) return false;
	const LPWSTR raw = calloc(rawLength + 1, sizeof(WCHAR));
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
		Word word = { 0 };
		bool success = true;
		success = success && ReceiveString(multiplay, &word.Word);
		success = success && ReceiveString(multiplay, &word.Pronunciation);
		success = success && ReceiveString(multiplay, &word.Meaning);
		if (success) {
			AddWord(multiplay->Option->Vocabulary, &word);
		} else {
			DestroyWord(&word);
			DestroyVocabulary(multiplay->Option->Vocabulary);
			return false;
		}
	}
	return true;
}

bool SendHttpRequest(LPCSTR address, LPCSTR request, int requestLength, LPSTR response, int responseLength) {
	const SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == SOCKET_ERROR) return false;

	const PHOSTENT host = gethostbyname(address);
	if (!host) {
		closesocket(sock);
		return false;
	}

	SOCKADDR_IN server;
	server.sin_addr.s_addr = *(ULONG*)host->h_addr;
	server.sin_family = AF_INET;
	server.sin_port = htons(80);
	if (connect(sock, (SOCKADDR*)&server, sizeof(server))) {
		closesocket(sock);
		return false;
	}

	int result, processed = 0;
	do {
		result = send(sock, request + processed, requestLength - processed, 0);
		if (result == SOCKET_ERROR) {
			closesocket(sock);
			return false;
		}
		processed += result;
	} while (processed < requestLength);

	processed = 0;
	do {
		result = recv(sock, response + processed, responseLength - processed, 0);
		if (result == SOCKET_ERROR) {
			closesocket(sock);
			return false;
		}
		processed += result;
	} while (result > 0 && processed < responseLength);

	closesocket(sock);
	return true;
}