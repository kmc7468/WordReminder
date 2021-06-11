#include "Application.h"

#include "String.h"
#include "Window.h"
#include "Word.h"

#include <fcntl.h>
#include <io.h>
#include <locale.h>
#include <ShlObj.h>
#include <stdio.h>
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

	LoadSetting();

	g_GlobalFont.lfCharSet = HANGUL_CHARSET;
	if (Setting.FontName && _tcslen(Setting.FontName) < ARRAYSIZE(g_GlobalFont.lfFaceName) - 1) {
		_tcscpy(g_GlobalFont.lfFaceName, Setting.FontName);
	} else {
		_tcscpy(g_GlobalFont.lfFaceName, _T("�������"));
	}

	g_FileDialog.lpstrDefExt = _T("kv");
	g_FileDialog.lpstrFile = g_FileDialogPath;
	g_FileDialog.lpstrFilter = _T("�ܾ��� ����(*.kv, *.kwl)\0*.kv;*.kwl\0��� ����(*.*)\0*.*\0");
	g_FileDialog.lStructSize = sizeof(g_FileDialog);
	g_FileDialog.nMaxFile = ARRAYSIZE(g_FileDialogPath);

	TCHAR desktop[MAX_PATH];
	if (SHGetSpecialFolderPath(NULL, desktop, CSIDL_DESKTOP, FALSE)) {
		g_FileDialog.lpstrInitialDir = desktop;
	}

	MainWindow = CreateSceneWindow(MainWindowProc, MainSceneProc);

	WSADATA data;
	return WSAStartup(MAKEWORD(2, 2), &data) == ERROR_SUCCESS;
}
void DestroyApplication() {
	SaveSetting();
	free(Setting.FontName);
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

	// Global
	Setting.FontName = ReadString(key, _T("FontName"));
	Setting.Scale = ReadDWord(key, _T("Scale"), 100);

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

static void RunVocabularyEditor(FILE* stream, int argc, LPWSTR* argv);

bool ProcessCommandLineArguments(LPWSTR cmdArgs) {
	if (wcslen(cmdArgs) == 0) return false;

	int argc;
	LPWSTR* const argv = CommandLineToArgvW(cmdArgs, &argc);
	if (!argv) return false;

	FILE* stream = NULL;
	if (AttachConsole(ATTACH_PARENT_PROCESS)) {
		const HANDLE streamHandle = GetStdHandle(STD_OUTPUT_HANDLE);
		const int fd = _open_osfhandle((intptr_t)streamHandle, _O_TEXT);
		if (fd > 0) {
			stream = _fdopen(fd, "w");
			fputc('\n', stream);

			setlocale(LC_ALL, "");
		}
	}

	if (!stream) {
		LocalFree(argv);
		return false;
	}

	if (wcscmp(argv[0], L"vocabulary") == 0) {
		RunVocabularyEditor(stream, argc, argv);
	} else {
		fprintf(stream,
			"����: ./WordReminder.exe <����> [�μ�...]\n\n"
			"����:\n"
			"  vocabulary        �ܾ��� ����\n");
	}

	LocalFree(argv);
	return true;
}

void RunVocabularyEditor(FILE* stream, int argc, LPWSTR* argv) {
	if (argc > 1 && wcscmp(argv[1], L"export") == 0) {
		if (argc == 2 || wcscmp(argv[2], L"help") == 0) {
			fprintf(stream,
				"����: ./WordReminder.exe vocabulary export <����> <�ܾ��� ���> <������ ���>\n\n"
				"����:\n"
				"  csv               UTF-8(BOM ����)�� ���ڵ��� csv �����Դϴ�.\n"
				"  csvs              UTF-8(BOM ������)�� ���ڵ��� csv �����Դϴ�.\n");
		} else if (argc == 5) {
			ExportType type;
			if (wcscmp(argv[2], L"csv") == 0) {
				type = Csv;
			} else if (wcscmp(argv[2], L"csvs") == 0) {
				type = CsvS;
			} else {
				fprintf(stream, "����: '%ws'�� �� �� ���� �����Դϴ�.", argv[2]);
				return;
			}

			const LPTSTR vocabularyPath = MakeGenericString(argv[3]);
			const LPTSTR exportPath = MakeGenericString(argv[4]);

			Vocabulary vocabulary = { 0 };
			CreateVocabulary(&vocabulary);
			if (LoadVocabulary(&vocabulary, vocabularyPath)) {
				if (ExportVocabulary(&vocabulary, type, exportPath)) {
					fprintf(stream, "����!\n");
				} else {
					fprintf(stream, "����: �ܾ����� �������� ���߽��ϴ�.\n");
				}
			} else {
				fprintf(stream, "����: �ܾ����� ���� ���߽��ϴ�.\n");
			}

			DestroyVocabulary(&vocabulary, true);

			FreeGenericString(vocabularyPath);
			FreeGenericString(exportPath);
		} else if (argc < 6) {
			fprintf(stream, "����: �μ��� �����մϴ�.\n");
		} else {
			fprintf(stream, "����: �μ��� �ʹ� �����ϴ�.\n");
		}
	} else if (argc > 1 && wcscmp(argv[1], L"merge") == 0) {
		if (argc == 2 || wcscmp(argv[2], L"help") == 0) {
			fprintf(stream,
				"����: ./WordReminder.exe vocabulary merge <��ĥ �ܾ��� 1 ���> <��ĥ �ܾ��� 2 ���> <������ ���>\n");
		} else if (argc == 5) {
			const LPTSTR vocabulary1Path = MakeGenericString(argv[2]);
			const LPTSTR vocabulary2Path = MakeGenericString(argv[3]);
			const LPTSTR exportPath = MakeGenericString(argv[4]);

			Vocabulary vocabulary1 = { 0 }, vocabulary2 = { 0 };
			CreateVocabulary(&vocabulary1);
			CreateVocabulary(&vocabulary2);

			if (!LoadVocabulary(&vocabulary1, vocabulary1Path)) {
				fprintf(stream, "����: �ܾ��� 1�� ���� ���߽��ϴ�.\n");
			} else if (!LoadVocabulary(&vocabulary2, vocabulary2Path)) {
				fprintf(stream, "����: �ܾ��� 2�� ���� ���߽��ϴ�.\n");
			} else {
				for (int i = 0; i < vocabulary2.Words.Count; ++i) {
					Word* const word = GetWord(&vocabulary2, i);
					const int wordIndex = FindWord(&vocabulary1, word->Word);
					if (wordIndex == -1) {
						Word copied = { 0 };
						CopyWord(&copied, word);
						AddWord(&vocabulary1, &copied);
					} else {
						Word* const destination = GetWord(&vocabulary1, wordIndex);
						for (int j = 0; j < word->Meanings.Count; ++j) {
							Meaning* const meaning = GetMeaning(word, j);
							const int meaningIndex = FindMeaning(destination, meaning->Meaning);
							if (meaningIndex == -1) {
								Meaning copied = { 0 };
								CopyMeaning(&copied, meaning);
								AddMeaning(destination, &copied);
							} else {
								Meaning* const crashed = GetMeaning(destination, meaningIndex);
								if (_tcscmp(meaning->Pronunciation, crashed->Pronunciation) == 0) continue;

								const bool hasPronunciation = (_tcslen(meaning->Pronunciation) != 0 &&
									_tcscmp(GetWord(&vocabulary2, meaning->Word), meaning->Pronunciation) != 0);
								const bool crashedHasPronunciation = (_tcslen(crashed->Pronunciation) != 0 &&
									_tcscmp(GetWord(&vocabulary1, crashed->Word), crashed->Pronunciation) != 0);
								if (hasPronunciation && !crashedHasPronunciation) {
									crashed->Pronunciation = meaning->Pronunciation;
									meaning->Pronunciation = NULL;
								} else if (hasPronunciation && crashedHasPronunciation) {
									const LPWSTR wordRawStr = GetRawString(word->Word);
									const LPWSTR meaningRawStr = GetRawString(meaning->Meaning);
									fprintf(stream,
										"����: �ܾ� '%ws'�� �� '%ws'�� ��ĥ �� �����ϴ�.\n"
										"����: �ܾ��� 1�� %d��°, �ܾ��� 2�� %d��° �ܾ��Դϴ�.\n"
										"����: ������ �ٸ��ϴ�.\n",
										wordRawStr, meaningRawStr, wordIndex + 1, i + 1);

									FreeRawString(wordRawStr);
									FreeRawString(meaningRawStr);
									goto end;
								}
							}
						}
					}
				}

				if (SaveVocabulary(&vocabulary1, exportPath)) {
					fprintf(stream, "����!\n");
				} else {
					fprintf(stream, "����: �ܾ����� �������� ���߽��ϴ�.\n");
				}
			}

		end:
			DestroyVocabulary(&vocabulary1, true);
			DestroyVocabulary(&vocabulary2, true);

			FreeGenericString(vocabulary1Path);
			FreeGenericString(vocabulary2Path);
			FreeGenericString(exportPath);
		} else if (argc < 6) {
			fprintf(stream, "����: �μ��� �����մϴ�.\n");
		} else {
			fprintf(stream, "����: �μ��� �ʹ� �����ϴ�.\n");
		}
	} else {
		fprintf(stream,
			"����: ./WordReminder.exe vocabulary <���> [�μ�...]\n\n"
			"���:\n"
			"  export            �ܾ����� �ٸ� �������� �������ϴ�.\n"
			"  merge             �� �ܾ����� �ϳ��� �ܾ������� ��Ĩ�ϴ�.\n");
	}
}