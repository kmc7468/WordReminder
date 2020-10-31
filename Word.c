#include "Word.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>

static LPSTR WCS2MBS(LPCWSTR wcs, bool destroyWCS);
static LPWSTR MBS2WCS(LPCSTR mbs, bool destroyMBS);
static LPTSTR ReadString(FILE* file);
static void WriteString(FILE* file, LPCTSTR string);

void DestroyWord(Word* word) {
	free(word->Word);
	free(word->Pronunciation);
	free(word->Meaning);
}

bool LoadVocabulary(Vocabulary* vocabulary, LPCTSTR path) {
	FILE* const file = _tfopen(path, _T("rb"));
	if (!file) return false;

	int count;
	fread(&count, sizeof(count), 1, file);
	for (int i = 0; i < count; ++i) {
		Word word;
		word.Word = ReadString(file);
		word.Pronunciation = ReadString(file);
		word.Meaning = ReadString(file);
		word.IsWrong = false;
		AddWord(vocabulary, &word);
	}
	fclose(file);
	return true;
}
bool SaveVocabulary(const Vocabulary* vocabulary, LPCTSTR path) {
	FILE* const file = _tfopen(path, _T("wb"));
	if (!file) return false;

	fwrite(&vocabulary->Count, sizeof(vocabulary->Count), 1, file);
	for (int i = 0; i < vocabulary->Count; ++i) {
		WriteString(file, vocabulary->Array[i].Word);
		WriteString(file, vocabulary->Array[i].Pronunciation);
		WriteString(file, vocabulary->Array[i].Meaning);
	}
	fclose(file);
	return true;
}
bool AddWord(Vocabulary* vocabulary, const Word* word) {
	if (vocabulary->Capacity == vocabulary->Count) {
		const int newCapacity = max(1, vocabulary->Capacity * 2);
		Word* const newArray = realloc(vocabulary->Array, sizeof(Word) * newCapacity);
		if (!newArray) return false;

		vocabulary->Array = newArray;
		vocabulary->Capacity = newCapacity;
	}
	vocabulary->Array[vocabulary->Count++] = *word;
	return true;
}
void RemoveWord(Vocabulary* vocabulary, int index) {
	DestroyWord(vocabulary->Array + index);
	memmove(vocabulary->Array + index, vocabulary->Array + index + 1, sizeof(Word) * (--vocabulary->Count - index));
}
void CopyWord(Word* dest, const Word* source) {
	_tcscpy(dest->Word = malloc(sizeof(TCHAR) * 201), source->Word);
	_tcscpy(dest->Pronunciation = malloc(sizeof(TCHAR) * 201), source->Pronunciation);
	_tcscpy(dest->Meaning = malloc(sizeof(TCHAR) * 201), source->Meaning);
	dest->IsWrong = source->IsWrong;
}
int GetUniqueWordCount(const Vocabulary* vocabulary) {
	int result = vocabulary->Count;
	for (int i = 0; i < vocabulary->Count; ++i) {
		for (int j = 0; j < i; ++j) {
			if (!_tcscmp(vocabulary->Array[i].Meaning, vocabulary->Array[j].Meaning)) {
				--result;
				break;
			}
		}
	}
	return result;
}
void DestroyVocabulary(Vocabulary* vocabulary) {
	for (int i = 0; i < vocabulary->Count; ++i) {
		DestroyWord(vocabulary->Array + i);
	}
	free(vocabulary->Array);
	memset(vocabulary, 0, sizeof(*vocabulary));
}

LPSTR WCS2MBS(LPCWSTR wcs, bool destroyWCS) {
	const int length = WideCharToMultiByte(CP_ACP, 0, wcs, -1, NULL, 0, NULL, NULL);
	const LPSTR result = malloc(sizeof(CHAR) * length);
	WideCharToMultiByte(CP_ACP, 0, wcs, -1, result, length, NULL, NULL);
	if (destroyWCS) {
		free((LPWSTR)wcs);
	}
	return result;
}
LPWSTR MBS2WCS(LPCSTR mbs, bool destroyMBS) {
	const int length = MultiByteToWideChar(CP_ACP, 0, mbs, -1, NULL, 0);
	const LPWSTR result = malloc(sizeof(WCHAR) * length);
	MultiByteToWideChar(CP_ACP, 0, mbs, -1, result, length);
	if (destroyMBS) {
		free((LPSTR)mbs);
	}
	return result;
}
LPTSTR ReadString(FILE* file) {
	int length;
	fread(&length, sizeof(length), 1, file);
	const LPWSTR rawStr = malloc(sizeof(WCHAR) * (length + 1));
	fread(rawStr, sizeof(*rawStr), length, file);
	rawStr[length] = 0;
#if defined(UNICODE) || defined(_UNICODE)
	return rawStr;
#else
	return WCS2MBS(rawStr, true);
#endif
}
void WriteString(FILE* file, LPCTSTR string) {
	const LPCWSTR raw =
#if defined(UNICODE) || defined(_UNICODE)
		string;
#else
		MBS2WCS(string, false);
#endif
	const int rawLength = (int)wcslen(raw);
	fwrite(&rawLength, sizeof(rawLength), 1, file);
	fwrite(raw, sizeof(WCHAR), rawLength, file);
#if !defined(UNICODE) && !defined(_UNICODE)
	free((LPWSTR)raw);
#endif
}

void GenerateQuestion(Question* question, const QuestionOption* option) {
	for (int i = 0; i < 5; ++i) {
		bool unique = false;
		do {
			question->Words[i] = option->Vocabulary.Array + rand() % option->Vocabulary.Count;
			for (int j = 0; j <= i; ++j) {
				if (i == j) {
					unique = true;
				} else if (!_tcscmp(question->Words[i]->Meaning, question->Words[j]->Meaning)) break;
			}
		} while (!unique);
	}

	if (option->QuestionType == GuessingBoth) {
		question->Type = (QuestionType)(rand() % 2 + 1);
	} else {
		question->Type = option->QuestionType;
	}

	int newAnswer;
	do {
		newAnswer = rand() % 5;
	} while (newAnswer == question->Answer);
	question->Answer = newAnswer;
}