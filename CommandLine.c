#include "CommandLine.h"

#include "Application.h"
#include "String.h"
#include "Word.h"

#include <fcntl.h>
#include <io.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <varargs.h>

static void PrintUsage(const char* template);
static void PrintMenu(const char* category, ...);

#define CheckString(a, b) (wcscmp(a, b) == 0)
#define CheckMenu(index, string) (argc > index && CheckString(argv[index], string))

#define ERR_TOO_MANY_ARGUMENTS "����: �μ��� �ʹ� �����ϴ�.\n"
#define ERR_NOT_ENOUGH_ARGUMENTS "����: �μ��� �����մϴ�.\n"
#define ERR_FAILED_TO_OPEN_VOCABULARY "����: �ܾ����� ���� ���߽��ϴ�.\n"
#define ERR_FAILED_TO_EXPORT_VOCABULARY "����: �ܾ����� �������� ���߽��ϴ�.\n"
#define INF_SUCCESS "����!\n"

static void RunOptionEditor(int argc, LPWSTR* argv);
static void PrintOption(LPWSTR option);
static void ChangeOption(LPWSTR option, LPWSTR newValue);

static void RunVocabularyEditor(int argc, LPWSTR* argv);
static void EditWord(LPTSTR vocabularyPath, LPTSTR word, LPTSTR newWord);
static void ExportVocabularyI(LPWSTR type, LPTSTR vocabularyPath, LPTSTR exportPath);
static void MergeVocabulary(LPTSTR vocabulary1Path, LPTSTR vocabulary2Path, LPTSTR exportPath);

bool ProcessCommandLineArguments(LPWSTR cmdArgs) {
	if (wcslen(cmdArgs) == 0) return false;

	int argc;
	LPWSTR* const argv = CommandLineToArgvW(cmdArgs, &argc);
	if (!argv) return false;

	if (AllocConsole()) {
		freopen("CONOUT$", "w", stdout);
	} else {
		LocalFree(argv);
		return false;
	}

	if (CheckMenu(0, L"option")) {
		RunOptionEditor(argc, argv);
	} else if (CheckMenu(0, L"vocabulary")) {
		RunVocabularyEditor(argc, argv);
	} else {
		PrintUsage("<����> [�μ�...]");
		PrintMenu("����",
			"option", "���� ������ �����մϴ�.",
			"vocabulary", "�ܾ����� �����մϴ�.", NULL);
	}

	putc('\n', stdout);
	system("pause");

	LocalFree(argv);
	FreeConsole();
	return true;
}

void PrintUsage(const char* template) {
	printf("����: ./WordReminder.exe %s\n", template);
}
void PrintMenu(const char* category, ...) {
	va_list ap;
	va_start(ap, category);

	printf("\n%s:\n", category);

	const char* menu;
	while ((menu = va_arg(ap, const char*))) {
		printf("  %-18s%s\n", menu, va_arg(ap, const char*));
	}
	va_end(ap);
}

void RunOptionEditor(int argc, LPWSTR* argv) {
	if (CheckMenu(1, L"fontname") || CheckMenu(1, L"scale")) {
		if (argc == 2) {
			PrintOption(argv[1]);
		} else if (argc == 3) {
			ChangeOption(argv[1], argv[2]);
		} else {
			printf(ERR_TOO_MANY_ARGUMENTS);
		}
	} else {
		PrintUsage("option <����> [�� ��]");
		PrintMenu("����",
			"fontname", "��Ʈ �̸��Դϴ�(�⺻��: NULL).",
			"scale", "�߰� ȭ�� ����(%%)�Դϴ�(�⺻��: 100).", NULL);
	}
}
void PrintOption(LPWSTR option) {
	if (CheckString(option, L"fontname")) {
		if (Setting.FontName) {
			const LPCWSTR fontNameRaw = GetRawString(Setting.FontName);
			printf("���� ��: '%ws'\n", fontNameRaw);
			FreeRawString(fontNameRaw);
		} else {
			printf("���� ��: NULL\n");
		}
	} else if (CheckString(option, L"scale")) {
		printf("���� ��: %d\n", Setting.Scale);
	}
}
void ChangeOption(LPWSTR option, LPWSTR newValue) {
	if (CheckString(option, L"fontname")) {
		free(Setting.FontName);

		if (CheckString(newValue, L"NULL")) {
			Setting.FontName = NULL;
		} else {
			Setting.FontName = MakeGenericString(newValue);
		}
	} else if (CheckString(option, L"scale")) {
		int newScale;
		if (swscanf(newValue, L"%d", &newScale) != 1) {
			printf("����: �� ���� ������ �ƴմϴ�.\n");
			return;
		} else if (newScale <= 0) {
			printf("����: �� ���� 0 �Ǵ� ������ �� �����ϴ�.\n");
			return;
		}

		Setting.Scale = newScale;
	}

	SaveSetting();
	printf(INF_SUCCESS);
}

void RunVocabularyEditor(int argc, LPWSTR* argv) {
	if (CheckMenu(1, L"editword")) {
		if (argc == 2 || CheckMenu(2, L"help")) {
			PrintUsage("vocabulary editword <�ܾ��� ���> <�ܾ�> <�� �ܾ�>");
		} else if (argc == 5) {
			const LPTSTR vocabularyPath = MakeGenericString(argv[2]);
			const LPTSTR word = MakeGenericString(argv[3]);
			const LPTSTR newWord = MakeGenericString(argv[4]);
			EditWord(vocabularyPath, word, newWord);

			FreeGenericString(vocabularyPath);
			FreeGenericString(word);
			FreeGenericString(newWord);
		} else if (argc < 6) {
			printf(ERR_NOT_ENOUGH_ARGUMENTS);
		} else {
			printf(ERR_TOO_MANY_ARGUMENTS);
		}
	} else if (CheckMenu(1, L"export")) {
		if (argc == 2 || CheckMenu(2, L"help")) {
			PrintUsage("vocabulary export <����> <�ܾ��� ���> <������ ���>");
			PrintMenu("����",
				"csv", "UTF-8(BOM ����)�� ���ڵ��� csv �����Դϴ�.",
				"csvs", "UTF-8(BOM ������)�� ���ڵ��� csv �����Դϴ�.", NULL);
		} else if (argc == 5) {
			const LPTSTR vocabularyPath = MakeGenericString(argv[3]);
			const LPTSTR exportPath = MakeGenericString(argv[4]);
			ExportVocabularyI(argv[2], vocabularyPath, exportPath);

			FreeGenericString(vocabularyPath);
			FreeGenericString(exportPath);
		} else if (argc < 6) {
			printf(ERR_NOT_ENOUGH_ARGUMENTS);
		} else {
			printf(ERR_TOO_MANY_ARGUMENTS);
		}
	} else if (CheckMenu(1, L"merge")) {
		if (argc == 2 || CheckMenu(2, L"help")) {
			PrintUsage("vocabulary merge <��ĥ �ܾ��� 1 ���> <��ĥ �ܾ��� 2 ���> <������ ���>");
		} else if (argc == 5) {
			const LPTSTR vocabulary1Path = MakeGenericString(argv[2]);
			const LPTSTR vocabulary2Path = MakeGenericString(argv[3]);
			const LPTSTR exportPath = MakeGenericString(argv[4]);
			MergeVocabulary(vocabulary1Path, vocabulary2Path, exportPath);

			FreeGenericString(vocabulary1Path);
			FreeGenericString(vocabulary2Path);
			FreeGenericString(exportPath);
		} else if (argc < 6) {
			printf(ERR_NOT_ENOUGH_ARGUMENTS);
		} else {
			printf(ERR_TOO_MANY_ARGUMENTS);
		}
	} else {
		PrintUsage("vocabulary <���> [�μ�...]");
		PrintMenu("���",
			"editword", "�ܾ �����մϴ�.",
			"export", "�ܾ����� �ٸ� �������� �������ϴ�.",
			"merge", "�� �ܾ����� �ϳ��� �ܾ������� ��Ĩ�ϴ�.", NULL);
	}
}
void EditWord(LPTSTR vocabularyPath, LPTSTR word, LPTSTR newWord) {
	Vocabulary vocabulary = { 0 };
	CreateVocabulary(&vocabulary);
	if (LoadVocabulary(&vocabulary, vocabularyPath)) {
		const int wordIndex = FindWord(&vocabulary, word);
		const int newWordIndex = FindWord(&vocabulary, newWord);
		if (wordIndex == -1) {
			_tprintf(_T("����: �ܾ� '%Ts'�� ã�� ���߽��ϴ�.\n"), word);
		} else if (newWordIndex != -1) {
			_tprintf(_T("����: �ܾ� '%Ts'�� �̹� �����մϴ�.\n"), newWord);
		} else {
			LPTSTR* target = &GetWord(&vocabulary, wordIndex)->Word;
			free(*target);

			*target = malloc(sizeof(TCHAR) * (_tcslen(newWord) + 1));
			_tcscpy(*target, newWord);

			if (SaveVocabulary(&vocabulary, vocabularyPath)) {
				printf(INF_SUCCESS);
			} else {
				printf(ERR_FAILED_TO_EXPORT_VOCABULARY);
			}
		}
	} else {
		printf(ERR_FAILED_TO_OPEN_VOCABULARY);
	}

	DestroyVocabulary(&vocabulary, true);
}
void ExportVocabularyI(LPWSTR type, LPTSTR vocabularyPath, LPTSTR exportPath) {
	ExportType typeEnum;
	if (CheckString(type, L"csv")) {
		typeEnum = Csv;
	} else if (CheckString(type, L"csvs")) {
		typeEnum = CsvS;
	} else {
		printf("����: '%ws'�� �� �� ���� �����Դϴ�.", type);
		return;
	}

	Vocabulary vocabulary = { 0 };
	CreateVocabulary(&vocabulary);
	if (LoadVocabulary(&vocabulary, vocabularyPath)) {
		if (ExportVocabulary(&vocabulary, typeEnum, exportPath)) {
			printf(INF_SUCCESS);
		} else {
			printf(ERR_FAILED_TO_EXPORT_VOCABULARY);
		}
	} else {
		printf(ERR_FAILED_TO_OPEN_VOCABULARY);
	}

	DestroyVocabulary(&vocabulary, true);
}
void MergeVocabulary(LPTSTR vocabulary1Path, LPTSTR vocabulary2Path, LPTSTR exportPath) {
	Vocabulary vocabulary1 = { 0 }, vocabulary2 = { 0 };
	CreateVocabulary(&vocabulary1);
	CreateVocabulary(&vocabulary2);

	if (!LoadVocabulary(&vocabulary1, vocabulary1Path)) {
		printf("����: �ܾ��� 1�� ���� ���߽��ϴ�.\n");
	} else if (!LoadVocabulary(&vocabulary2, vocabulary2Path)) {
		printf("����: �ܾ��� 2�� ���� ���߽��ϴ�.\n");
	} else {
		for (int i = 0; i < vocabulary2.Words.Count; ++i) {
			Word* const source = GetWord(&vocabulary2, i);
			const int destIndex = FindWord(&vocabulary1, source->Word);
			if (destIndex == -1) {
				Word copied = { 0 };
				CopyWord(&copied, source);
				AddWord(&vocabulary1, &copied);
				continue;
			}

			Word* const dest = GetWord(&vocabulary1, destIndex);
			for (int j = 0; j < source->Meanings.Count; ++j) {
				Meaning* const sourceMeaning = GetMeaning(source, j);
				const int crashIndex = FindMeaning(dest, sourceMeaning->Meaning);
				if (crashIndex == -1) {
					Meaning copied = { 0 };
					CopyMeaning(&copied, sourceMeaning);
					AddMeaning(dest, &copied);
					continue;
				}

				Meaning* const crash = GetMeaning(dest, crashIndex);
				if (_tcscmp(sourceMeaning->Pronunciation, crash->Pronunciation) == 0) continue;

				const bool hasPronunciation = (_tcslen(sourceMeaning->Pronunciation) != 0 &&
					_tcscmp(GetWord(&vocabulary2, sourceMeaning->Word)->Word, sourceMeaning->Pronunciation) != 0);
				const bool crashHasPronunciation = (_tcslen(crash->Pronunciation) != 0 &&
					_tcscmp(GetWord(&vocabulary1, crash->Word)->Word, crash->Pronunciation) != 0);
				if (hasPronunciation && !crashHasPronunciation) {
					crash->Pronunciation = sourceMeaning->Pronunciation;
					sourceMeaning->Pronunciation = NULL;
				} else if (hasPronunciation && crashHasPronunciation) {
					_tprintf(
						_T("����: �ܾ� '%Ts'�� �� '%Ts'�� ��ĥ �� �����ϴ�.\n")
						_T("����: �ܾ��� 1�� %d��°, �ܾ��� 2�� %d��° �ܾ��Դϴ�.\n")
						_T("����: ������ �ٸ��ϴ�.\n"),
						source->Word, sourceMeaning->Meaning, destIndex + 1, i + 1);
					goto end;
				}
			}
		}

		if (!IsUsableVocabulary(&vocabulary1, GuessMeaning, 0)) {
			printf(
				"����: �ܾ����� ��ĥ �� �����ϴ�.\n"
				"����: �ٸ� �ܾ�� ���� ������ ���� ���� �ܾ��� ������ 5�� �̸��Դϴ�.\n");
		} else if (SaveVocabulary(&vocabulary1, exportPath)) {
			printf(INF_SUCCESS);
		} else {
			printf(ERR_FAILED_TO_EXPORT_VOCABULARY);
		}
	}

end:
	DestroyVocabulary(&vocabulary1, true);
	DestroyVocabulary(&vocabulary2, true);
}