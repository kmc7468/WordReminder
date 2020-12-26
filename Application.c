#include "Application.h"

#include <stdlib.h>
#include <string.h>

RegistryData Setting;

static DWORD ReadDWord(HKEY key, LPCTSTR name, DWORD defaultValue);
static void WriteDWord(HKEY key, LPCTSTR name, DWORD value);
static LPTSTR ReadString(HKEY key, LPCTSTR name);
static void WriteString(HKEY key, LPCTSTR name, LPCTSTR value);

void LoadSetting() {
	HKEY key;
	if (RegOpenKey(HKEY_CURRENT_USER, _T("SOFTWARE\\Staticom\\WordReminder"), &key) != ERROR_SUCCESS) return;

	// QuestionOptionScene
	Setting.GuessMeaning = (bool)ReadDWord(key, _T("GuessMeaning"), false);
	Setting.GuessWord = (bool)ReadDWord(key, _T("GuessWord"), false);
	Setting.GuessPronunciation = (bool)ReadDWord(key, _T("GuessPronunciation"), false);

	Setting.ShowPronunciation = (bool)ReadDWord(key, _T("GivePronunciation"), false); // Legacy
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
	WriteDWord(key, _T("GuessWord"), Setting.GuessWord);
	WriteDWord(key, _T("GuessPronunciation"), Setting.GuessPronunciation);

	WriteDWord(key, _T("GivePronunciation"), Setting.ShowPronunciation); // Legacy
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
void WriteDWord(HKEY key, LPCTSTR name, DWORD value) {
	RegSetValueEx(key, name, 0, REG_DWORD, (LPBYTE)&value, sizeof(value));
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
void WriteString(HKEY key, LPCTSTR name, LPCTSTR value) {
	if (value) {
		RegSetValueEx(key, name, 0, REG_SZ, (LPBYTE)value, (DWORD)(sizeof(TCHAR) * _tcslen(value)));
	}
}