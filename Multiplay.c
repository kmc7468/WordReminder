#include "Multiplay.h"

#include <stdlib.h>
#include <string.h>

#define My multiplay->Players[0]

bool OpenServer(Multiplay* multiplay, MultiplayOption* multiplayOption) {
	multiplay->Option = multiplayOption;
	memset(multiplay->Players, 0, sizeof(multiplay->Players));

	if ((My.Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == SOCKET_ERROR) return false;

	multiplay->ServerAddress.sin_addr.s_addr = INADDR_ANY;
	multiplay->ServerAddress.sin_family = AF_INET;
	multiplay->ServerAddress.sin_port = htons((u_short)multiplayOption->ServerPort);
	if (bind(My.Socket, (SOCKADDR*)&multiplay->ServerAddress, sizeof(multiplay->ServerAddress))) {
		closesocket(My.Socket);
		return false;
	} else return true;
}
bool JoinServer(Multiplay* multiplay, MultiplayOption* multiplayOption) {
	multiplay->Option = multiplayOption;
	memset(multiplay->Players, 0, sizeof(multiplay->Players));

	if ((My.Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == SOCKET_ERROR) return false;

	const PHOSTENT host = gethostbyname(multiplayOption->ServerIp);

	multiplay->ServerAddress.sin_addr = *(IN_ADDR*)host->h_addr_list[0];
	multiplay->ServerAddress.sin_family = AF_INET;
	multiplay->ServerAddress.sin_port = htons((u_short)multiplayOption->ServerPort);
	if (connect(My.Socket, (SOCKADDR*)&multiplay->ServerAddress, sizeof(multiplay->ServerAddress))) {
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