#include "Version.h"

static DWORD ReadRegistryDWord(HKEY key, LPCTSTR name, DWORD defaultValue);
static void WriteRegistryDWord(HKEY key, LPCTSTR name, DWORD value);
static LPTSTR ReadRegistryString(HKEY key, LPCTSTR name);
static void WriteRegistryString(HKEY key, LPCTSTR name, LPCTSTR value);

RegistryData Setting;

void LoadSetting() {
	HKEY key;
	RegOpenKey(HKEY_CURRENT_USER, _T("SOFTWARE\\Staticom\\WordReminder"), &key);

	Setting.GuessMeaning = (bool)ReadRegistryDWord(key, _T("GuessMeaning"), false);
	Setting.GuessWord = (bool)ReadRegistryDWord(key, _T("GuessWord"), false);
	Setting.GivePronunciation = (bool)ReadRegistryDWord(key, _T("GivePronunciation"), false);
	Setting.ExcludeDuplicatedAnswer = (bool)ReadRegistryDWord(key, _T("ExcludeDuplicatedAnswer"), false);

	Setting.NewServerPort = (int)ReadRegistryDWord(key, _T("NewServerPort"), 1234);
	Setting.NewServerMode = (OnlineMultiplayMode)ReadRegistryDWord(key, _T("NewServerMode"), TurnMode);
	Setting.NewServerRole = (OnlineMultiplayRole)ReadRegistryDWord(key, _T("NewServerRole"), Examiner);

	Setting.ServerIp = ReadRegistryString(key, _T("ServerIp"));
	Setting.ServerPort = (int)ReadRegistryDWord(key, _T("ServerPort"), 1234);

	RegCloseKey(key);
}
void SaveSetting() {
	HKEY key;
	if (RegOpenKey(HKEY_CURRENT_USER, _T("SOFTWARE\\Staticom\\WordReminder"), &key) != ERROR_SUCCESS &&
		RegCreateKey(HKEY_CURRENT_USER, _T("SOFTWARE\\Staticom\\WordReminder"), &key) != ERROR_SUCCESS) return;

	WriteRegistryDWord(key, _T("GuessMeaning"), Setting.GuessMeaning);
	WriteRegistryDWord(key, _T("GuessWord"), Setting.GuessWord);
	WriteRegistryDWord(key, _T("GivePronunciation"), Setting.GivePronunciation);
	WriteRegistryDWord(key, _T("ExcludeDuplicatedAnswer"), Setting.ExcludeDuplicatedAnswer);

	WriteRegistryDWord(key, _T("NewServerPort"), Setting.NewServerPort);
	WriteRegistryDWord(key, _T("NewServerMode"), Setting.NewServerMode);
	WriteRegistryDWord(key, _T("NewServerRole"), Setting.NewServerRole);

	WriteRegistryString(key, _T("ServerIp"), Setting.ServerIp);
	WriteRegistryDWord(key, _T("ServerPort"), Setting.ServerPort);

	RegCloseKey(key);
}

DWORD ReadRegistryDWord(HKEY key, LPCTSTR name, DWORD defaultValue) {
	DWORD buffer, bufferSize = sizeof(buffer), dataType;
	if (RegQueryValueEx(key, name, NULL, &dataType, (LPBYTE)&buffer, &bufferSize) == ERROR_SUCCESS && dataType == REG_DWORD) return buffer;
	else return defaultValue;
}
void WriteRegistryDWord(HKEY key, LPCTSTR name, DWORD value) {
	RegSetValueEx(key, name, 0, REG_DWORD, (BYTE*)&value, sizeof(value));
}
LPTSTR ReadRegistryString(HKEY key, LPCTSTR name) {
	TCHAR dummy[1];
	DWORD bufferSize = sizeof(dummy), dataType;
	if (RegQueryValueEx(key, name, NULL, &dataType, (LPBYTE)dummy, &bufferSize) != ERROR_MORE_DATA || dataType != REG_SZ) return NULL;

	const LPTSTR buffer = malloc(sizeof(TCHAR) * bufferSize);
	if (RegQueryValueEx(key, name, NULL, &dataType, (LPBYTE)buffer, &bufferSize) == ERROR_SUCCESS) return buffer;
	else {
		free(buffer);
		return NULL;
	}
}
void WriteRegistryString(HKEY key, LPCTSTR name, LPCTSTR value) {
	if (value) {
		RegSetValueEx(key, name, 0, REG_SZ, (BYTE*)value, (DWORD)(sizeof(TCHAR) * _tcslen(value)));
	}
}