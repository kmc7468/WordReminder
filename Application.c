#include "Application.h"

#include "Window.h"

#include <ShlObj.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static LOGFONT g_GlobalFont;

static OPENFILENAME g_FileDialog;
static TCHAR g_FileDialogPath[MAX_PATH];

HINSTANCE Instance;

bool InitializeApplication(HINSTANCE instance) {
	Instance = instance;
	srand((unsigned)time(NULL));

	if (IsHiDPIAvailable()) {
		SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
	}

	RegisterWindow(_T("SceneWindow"), SceneWindowProc);
	RegisterWindow(_T("Scene"), SceneProc);

	g_GlobalFont.lfCharSet = HANGUL_CHARSET;
	_tcscpy(g_GlobalFont.lfFaceName, _T("나눔고딕"));

	g_FileDialog.lpstrDefExt = _T("kv");
	g_FileDialog.lpstrFile = g_FileDialogPath;
	g_FileDialog.lpstrFilter = _T("단어장 파일(*.kv, *.kwl)\0*.kv;*.kwl\0모든 파일(*.*)\0*.*\0");
	g_FileDialog.lStructSize = sizeof(g_FileDialog);
	g_FileDialog.nMaxFile = ARRAYSIZE(g_FileDialogPath);

	TCHAR desktop[MAX_PATH];
	if (SHGetSpecialFolderPath(NULL, desktop, CSIDL_DESKTOP, FALSE)) {
		g_FileDialog.lpstrInitialDir = desktop;
	}

	LoadSetting();

	MainWindow = CreateSceneWindow(MainWindowProc, MainSceneProc);

	WSADATA data;
	return WSAStartup(MAKEWORD(2, 2), &data) == ERROR_SUCCESS;
}
void DestroyApplication() {
	SaveSetting();
	free(Setting.ServerIp);

	WSACleanup();
}

HFONT CreateGlobalFont(int height, bool isBold) {
	g_GlobalFont.lfHeight = height;
	g_GlobalFont.lfWeight = isBold ? FW_BOLD : FW_NORMAL;
	return CreateFontIndirect(&g_GlobalFont);
}

LPCTSTR ShowOpenFileDialog(HWND window) {
	g_FileDialog.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST;
	g_FileDialog.hwndOwner = window;
	if (GetOpenFileName(&g_FileDialog)) return g_FileDialogPath;
	else return NULL;
}
LPCTSTR ShowSaveFileDialog(HWND window) {
	g_FileDialog.Flags = OFN_EXPLORER | OFN_HIDEREADONLY;
	g_FileDialog.hwndOwner = window;
	if (GetSaveFileName(&g_FileDialog)) return g_FileDialogPath;
	else return NULL;
}

RegistryData Setting;

static DWORD ReadDWord(HKEY key, LPCTSTR name, DWORD defaultValue);
static void WriteDWord(HKEY key, LPCTSTR name, DWORD data);
static LPTSTR ReadString(HKEY key, LPCTSTR name);
static void WriteString(HKEY key, LPCTSTR name, LPCTSTR data);

void LoadSetting() {
	HKEY key;
	if (RegOpenKey(HKEY_CURRENT_USER, _T("SOFTWARE\\Staticom\\WordReminder"), &key) != ERROR_SUCCESS) return;

	// QuestionOptionScene
	Setting.GuessMeaning = (bool)ReadDWord(key, _T("GuessMeaning"), false);
	Setting.GuessMeaningWithPronunciation = (int)ReadDWord(key, _T("GuessMeaningWithPronunciation"), 0);
	Setting.GuessWord = (bool)ReadDWord(key, _T("GuessWord"), false);
	Setting.GuessWordWithPronunciation = (int)ReadDWord(key, _T("GuessWordWithPronunciation"), 0);
	Setting.GuessPronunciation = (bool)ReadDWord(key, _T("GuessPronunciation"), false);

	Setting.ExcludeDuplicatedAnswer = (bool)ReadDWord(key, _T("ExcludeDuplicatedAnswer"), false);

	// OnlineMultiplayScene
	Setting.NewServerPort = (int)ReadDWord(key, _T("NewServerPort"), 1234);
	Setting.NewServerMode = (OnlineMultiplayMode)ReadDWord(key, _T("NewServerMode"), TurnBasedMode);
	Setting.NewServerRole = (OnlineMultiplayRole)ReadDWord(key, _T("NewServerRole"), Examiner);

	Setting.ServerIp = ReadString(key, _T("ServerIp"));
	Setting.ServerPort = (int)ReadDWord(key, _T("ServerPort"), 1234);

	RegCloseKey(key);
}
void SaveSetting() {
	HKEY key;
	if (RegOpenKey(HKEY_CURRENT_USER, _T("SOFTWARE\\Staticom\\WordReminder"), &key) != ERROR_SUCCESS &&
		RegCreateKey(HKEY_CURRENT_USER, _T("SOFTWARE\\Staticom\\WordReminder"), &key) != ERROR_SUCCESS) return;

	// QuestionOptionScene
	WriteDWord(key, _T("GuessMeaning"), Setting.GuessMeaning);
	WriteDWord(key, _T("GuessMeaningWithPronunciation"), Setting.GuessMeaningWithPronunciation);
	WriteDWord(key, _T("GuessWord"), Setting.GuessWord);
	WriteDWord(key, _T("GuessWordWithPronunciation"), Setting.GuessWordWithPronunciation);
	WriteDWord(key, _T("GuessPronunciation"), Setting.GuessPronunciation);

	WriteDWord(key, _T("ExcludeDuplicatedAnswer"), Setting.ExcludeDuplicatedAnswer);

	// OnlineMultiplayScene
	WriteDWord(key, _T("NewServerPort"), Setting.NewServerPort);
	WriteDWord(key, _T("NewServerMode"), Setting.NewServerMode);
	WriteDWord(key, _T("NewServerRole"), Setting.NewServerRole);

	WriteString(key, _T("ServerIp"), Setting.ServerIp);
	WriteDWord(key, _T("ServerPort"), Setting.ServerPort);

	RegCloseKey(key);
}

DWORD ReadDWord(HKEY key, LPCTSTR name, DWORD defaultValue) {
	DWORD buffer;
	DWORD bufferSize = sizeof(buffer);
	DWORD dataType;
	if (RegQueryValueEx(key, name, NULL, &dataType, (LPBYTE)&buffer, &bufferSize) == ERROR_SUCCESS && dataType == REG_DWORD) return buffer;
	else return defaultValue;
}
void WriteDWord(HKEY key, LPCTSTR name, DWORD data) {
	RegSetValueEx(key, name, 0, REG_DWORD, (LPBYTE)&data, sizeof(data));
}
LPTSTR ReadString(HKEY key, LPCTSTR name) {
	DWORD bufferSize;
	DWORD dataType;
	if (RegQueryValueEx(key, name, NULL, &dataType, NULL, &bufferSize) != ERROR_SUCCESS || dataType != REG_SZ) return NULL;

	const LPTSTR buffer = malloc(sizeof(TCHAR) * bufferSize);
	if (RegQueryValueEx(key, name, NULL, &dataType, (LPBYTE)buffer, &bufferSize) == ERROR_SUCCESS) return buffer;
	else {
		free(buffer);
		return NULL;
	}
}
void WriteString(HKEY key, LPCTSTR name, LPCTSTR data) {
	if (data) {
		RegSetValueEx(key, name, 0, REG_SZ, (LPBYTE)data, (DWORD)(sizeof(TCHAR) * _tcslen(data)));
	}
}

void StartThread(Thread* thread, LPTHREAD_START_ROUTINE function, LPVOID param) {
	if (thread->Handle) {
		DestroyThread(thread);
	}

	thread->Handle = CreateThread(NULL, 0, function, param, 0, &thread->Id);
}
void DestroyThread(Thread* thread) {
	CloseHandle(thread->Handle);
	thread->Handle = NULL;
}