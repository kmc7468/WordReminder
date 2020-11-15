#include "Multiplay.h"

#include "Window.h"

#include <stdlib.h>
#include <string.h>
#include <tchar.h>
#include <time.h>

#define My multiplay->Players[0]
#define Other multiplay->Players[1]

static SOCKET GetClientSocket(Multiplay* multiplay) {
	return multiplay->Option->SocketType == Server ? Other.Socket : My.Socket;
}

bool OpenServer(Multiplay* multiplay, MultiplayOption* multiplayOption) {
	multiplay->Option = multiplayOption;
	memset(multiplay->Players, 0, sizeof(multiplay->Players));
	multiplay->Status = OpeningServer;
	InvalidateRect(multiplay->Window, NULL, TRUE);

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
	multiplay->Status = WaitingForPlayer;
	InvalidateRect(multiplay->Window, NULL, TRUE);

	if (listen(My.Socket, 1) == SOCKET_ERROR) return false;
	else return (Other.Socket = accept(My.Socket, (SOCKADDR*)&Other.Address, NULL)) != SOCKET_ERROR;
}
bool JoinServer(Multiplay* multiplay, MultiplayOption* multiplayOption) {
	multiplay->Option = multiplayOption;
	memset(multiplay->Players, 0, sizeof(multiplay->Players));
	multiplay->Status = JoiningServer;
	InvalidateRect(multiplay->Window, NULL, TRUE);

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
	multiplay->Status = Singleplay;
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
	if (!SendInt(multiplay, multiplay->QuestionOption->Vocabulary.Count)) return false;
	for (int i = 0; i < multiplay->QuestionOption->Vocabulary.Count; ++i) {
		if (!SendString(multiplay, multiplay->QuestionOption->Vocabulary.Array[i].Word)) return false;
		if (!SendString(multiplay, multiplay->QuestionOption->Vocabulary.Array[i].Pronunciation)) return false;
		if (!SendString(multiplay, multiplay->QuestionOption->Vocabulary.Array[i].Meaning)) return false;
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
			AddWord(&multiplay->QuestionOption->Vocabulary, &word);
		} else {
			DestroyWord(&word);
			DestroyVocabulary(&multiplay->QuestionOption->Vocabulary);
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

typedef enum {
	StartCode,
	CompatibleCode,
	UncompatibleCode,
	ReadyCode,
	QuestionCode,
	AnswerCode,
	StopCode,
} ProtocolMagicNumber;

static Thread g_Thread;
static DWORD WINAPI WaitForPlayerThread(LPVOID param);
static DWORD WINAPI JoinServerThread(LPVOID param);
static DWORD WINAPI ReceiveThread(LPVOID param);

#define Context (*(Multiplay*)param)

void StartMultiplay(Multiplay* multiplay, MultiplayOption* option, Question* question, QuestionOption* questionOption, HWND handle) {
	multiplay->Window = handle;
	multiplay->Question = question;
	multiplay->QuestionOption = questionOption;
	if (option->SocketType == Server) {
		if (OpenServer(multiplay, option)) {
			StartThread(&g_Thread, WaitForPlayerThread, multiplay);
		} else {
			MessageBox(handle, _T("서버를 여는 중 오류가 발생했습니다."), _T("오류"), MB_OK | MB_ICONERROR);
			SendMessage(handle, WM_CLOSE, 0, 0);
		}
	} else {
		void** const temp = malloc(sizeof(void*) * 2);
		temp[0] = multiplay;
		temp[1] = option;
		StartThread(&g_Thread, JoinServerThread, temp);
	}
}
void StopMultiplay(Multiplay* multiplay) {
	SendInt(multiplay, StopCode);
	DestroyMultiplay(multiplay);
}
void SendQuestion(Multiplay* multiplay, HWND* buttons, int answer) {
	if (!SendInt(multiplay, QuestionCode) ||
		!SendInt(multiplay, multiplay->Question->Type)) {
		SendMessage(multiplay->Window, WM_USER + 5, 0, 0);
		return;
	}
	for (int i = 0; i < 5; ++i) {
		EnableWindow(buttons[i], FALSE); // TODO
		if (!SendInt(multiplay, (int)(multiplay->Question->Words[i] - multiplay->QuestionOption->Vocabulary.Array))) {
			SendMessage(multiplay->Window, WM_USER + 5, 0, 0);
			return;
		}
	}
	if (SendInt(multiplay, (multiplay->Question->Answer = answer))) {
		multiplay->Status = SentAnswer;
		InvalidateRect(multiplay->Window, NULL, TRUE);
	} else {
		SendMessage(multiplay->Window, WM_USER + 5, 0, 0);
	}
}
void SendAnswer(Multiplay* multiplay) {
	SendMessage(multiplay->Window, WM_USER + 5 - SendInt(multiplay, AnswerCode), 0, 0);
}

DWORD WINAPI WaitForPlayerThread(LPVOID param) {
	srand((unsigned)time(NULL));

	while (!WaitForPlayer(&Context));
	return ReceiveThread(param);
}
DWORD WINAPI JoinServerThread(LPVOID param) {
	void** const temp = (void**)param;
	MultiplayOption* const option = temp[1];
	param = temp[0];
	free(temp);

	if (!JoinServer(&Context, option)) {
		MessageBox(Context.Window, _T("서버에 접속하는 중 오류가 발생했습니다."), _T("오류"), MB_OK | MB_ICONERROR);
		SendMessage(Context.Window, WM_CLOSE, 0, 0);
		return 0;
	}

	srand((unsigned)time(NULL));

	LPTSTR serverVersion = NULL;
	int protocolVersion;
	if (!SendInt(&Context, StartCode) ||
		!ReceiveVersion(&Context, &serverVersion, &protocolVersion)) {
		free(serverVersion);
		goto Error;
	} else if (protocolVersion != WR_MULTIPLAY_PROTOCOL_VERSION) {
		const LPTSTR message = malloc((_tcslen(serverVersion) + 97) * sizeof(TCHAR));
		_tcscpy(message, _T("서버의 버전과 클라이언트의 버전이 호환되지 않습니다.\n"));
		_tcscat(message, serverVersion);
		_tcscat(message, _T(" 버전의 단어 암기 프로그램을 설치해 보세요."));
		MessageBox(Context.Window, message, _T("오류"), MB_OK | MB_ICONERROR);
		free(serverVersion);
		free(message);

		SendInt(&Context, UncompatibleCode);
		SendMessage(Context.Window, WM_CLOSE, 0, 0);
		return 0;
	}

	free(serverVersion);
	if (!SendInt(&Context, CompatibleCode) ||
		!ReceiveInt(&Context, (int*)&Context.QuestionOption->QuestionType) ||
		!ReceiveBool(&Context, &Context.QuestionOption->GivePronunciation) ||
		!ReceiveInt(&Context, (int*)&Context.Option->Mode) ||
		!ReceiveInt(&Context, (int*)&Context.Option->Role) ||
		!ReceiveVocabulary(&Context) ||
		!SendInt(&Context, ReadyCode)) {
	Error:
		MessageBox(Context.Window, _T("서버에 접속하는 중 오류가 발생했습니다."), _T("오류"), MB_OK | MB_ICONERROR);
		SendMessage(Context.Window, WM_CLOSE, 0, 0);
		return 0;
	} else {
		SendMessage(Context.Window, WM_USER + 2, 0, 0);
		return ReceiveThread(param);
	}
}
DWORD WINAPI ReceiveThread(LPVOID param) {
	int magic;
	while (ReceiveInt(&Context, &magic)) {
		if (magic == StartCode) {
			Context.Status = PlayerJoining;
			InvalidateRect(Context.Window, NULL, TRUE);
			if (!SendVersion(&Context)) goto ConnectionError;
		} else if (magic == CompatibleCode) {
			if (!SendInt(&Context, Context.QuestionOption->QuestionType) ||
				!SendBool(&Context, Context.QuestionOption->GivePronunciation) ||
				!SendInt(&Context, Context.Option->Mode) ||
				!SendInt(&Context, Context.Option->Role == Examiner ? Examinee : Examiner) ||
				!SendVocabulary(&Context)) goto ConnectionError;
		} else if (magic == UncompatibleCode) {
		ConnectionError:
			return WaitForPlayerThread(param);
		} else if (magic == ReadyCode) {
			SendMessage(Context.Window, WM_USER + 2, 0, 0);
		} else if (magic == QuestionCode) {
			if (!ReceiveInt(&Context, (int*)&Context.Question->Type)) goto Error;
			for (int i = 0; i < 5; ++i) {
				int index;
				if (!ReceiveInt(&Context, &index)) goto Error;
				Context.Question->Words[i] = Context.QuestionOption->Vocabulary.Array + index;
			}
			if (!ReceiveInt(&Context, &Context.Question->Answer)) goto Error;
			SendMessage(Context.Window, WM_USER + 3, 0, 0);
		} else if (magic == AnswerCode) {
			SendMessage(Context.Window, WM_USER + 4, 0, 0);
		} else {
			SendMessage(Context.Window, WM_USER + 6, 0, 0);
			return 0;
		}
	}

Error:
	SendMessage(Context.Window, WM_USER + 5, 0, 0);
	return 0;
}