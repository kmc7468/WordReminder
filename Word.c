#include "Word.h"

#include "String.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>

void CopyMeaning(Meaning* destination, const Meaning* source) {
	destination->Pronunciation = malloc(sizeof(TCHAR) * (_tcslen(source->Pronunciation) + 1));
	destination->Meaning = malloc(sizeof(TCHAR) * (_tcslen(source->Meaning) + 1));

	_tcscpy(destination->Pronunciation, source->Pronunciation);
	_tcscpy(destination->Meaning, source->Meaning);
	destination->Wrong = source->Wrong;
}
void DestroyMeaning(Meaning* meaning) {
	free(meaning->Pronunciation);
	free(meaning->Meaning);
}

void CreateWord(Word* word) {
	CreateArray(&word->Meanings, sizeof(Meaning));
}
void CopyWord(Word* destination, const Word* source) {
	CreateWord(destination);

	destination->Word = malloc(sizeof(TCHAR) * (_tcslen(source->Word) + 1));
	_tcscpy(destination->Word, source->Word);

	for (int i = 0; i < source->Meanings.Count; ++i) {
		Meaning copied;
		CopyMeaning(&copied, GetMeaning((Word*)source, i));
		AddMeaning(destination, &copied);
	}
}
void AddMeaning(Word* word, Meaning* meaning) {
	meaning->Word = word;

	AddElement(&word->Meanings, meaning);
}
void RemoveMeaning(Word* word, int index) {
	DestroyMeaning(GetMeaning(word, index));
	RemoveElement(&word->Meanings, index);
}
Meaning* GetMeaning(Word* word, int index) {
	return GetElement(&word->Meanings, index);
}
void DestroyWord(Word* word) {
	free(word->Word);
	DestroyArray(&word->Meanings);
}

static LPTSTR ReadString(FILE* file);
static void WriteString(FILE* file, LPCTSTR string);

void CreateVocabulary(Vocabulary* vocabulary) {
	CreateArray(&vocabulary->Words, sizeof(Word));
}
void CopyVocabulary(Vocabulary* destination, const Vocabulary* source) {
	CreateVocabulary(destination);

	for (int i = 0; i < source->Words.Count; ++i) {
		Word copied;
		CopyWord(&copied, GetWord((Vocabulary*)source, i));
		AddWord(destination, &copied);
	}
}
bool LoadVocabulary(Vocabulary* vocabulary, LPCTSTR path) {
	FILE* const file = _tfopen(path, _T("rb"));
	if (!file) return false;

	CreateVocabulary(vocabulary);

	int count;
	fread(&count, sizeof(count), 1, file);
	for (int i = 0; i < count; ++i) {
		Word word;
		CreateWord(&word);

		word.Word = ReadString(file);

		Meaning	legacyMeaning = { 0 };
		legacyMeaning.Pronunciation = ReadString(file);
		legacyMeaning.Meaning = ReadString(file);
		AddMeaning(&word, &legacyMeaning);

		AddWord(vocabulary, &word);
	}

	// TODO

	fclose(file);
	return true;
}
bool SaveVocabulary(const Vocabulary* vocabulary, LPCTSTR path) {
	FILE* const file = _tfopen(path, _T("wb"));
	if (!file) return false;

	fwrite(&vocabulary->Words.Count, sizeof(vocabulary->Words.Count), 1, file);
	for (int i = 0; i < vocabulary->Words.Count; ++i) {
		Word* const word = GetWord((Vocabulary*)vocabulary, i);

		WriteString(file, word->Word);

		int pronunciationLength = (word->Meanings.Count - 1) * 2;
		int meaningLength = pronunciationLength;
		for (int j = 0; j < word->Meanings.Count; ++j) {
			pronunciationLength += (int)_tcslen(GetMeaning(word, j)->Pronunciation);
			pronunciationLength += (int)_tcslen(GetMeaning(word, j)->Meaning);
		}

		const LPTSTR legacyPronunciation = calloc(pronunciationLength + 1, sizeof(TCHAR));
		const LPTSTR legacyMeaning = calloc(meaningLength + 1, sizeof(TCHAR));
		for (int j = 0; j < word->Meanings.Count; ++j) {
			if (j) {
				_tcscat(legacyPronunciation, _T(", "));
				_tcscat(legacyMeaning, _T(", "));
			}
			_tcscat(legacyPronunciation, GetMeaning(word, j)->Pronunciation);
			_tcscat(legacyMeaning, GetMeaning(word, j)->Meaning);
		}

		WriteString(file, legacyPronunciation);
		WriteString(file, legacyMeaning);
		free(legacyPronunciation);
		free(legacyMeaning);
	}

	// TODO

	fclose(file);
	return true;
}
void AddWord(Vocabulary* vocabulary, Word* word) {
	AddElement(&vocabulary->Words, word);
}
void RemoveWord(Vocabulary* vocabulary, int index) {
	DestroyWord(GetWord(vocabulary, index));
	RemoveElement(&vocabulary->Words, index);
}
Word* GetWord(Vocabulary* vocabulary, int index) {
	return GetElement(&vocabulary->Words, index);
}
void DestroyVocabulary(Vocabulary* vocabulary) {
	DestroyArray(&vocabulary->Words);
}

LPTSTR ReadString(FILE* file) {
	int length;
	fread(&length, sizeof(length), 1, file);
	const LPWSTR rawString = calloc(length + 1, sizeof(WCHAR));
	fread(rawString, sizeof(WCHAR), length, file);
	return MakeGenericString(rawString);
}
void WriteString(FILE* file, LPCTSTR string) {
	const LPCWSTR rawString = GetRawString(string);
	const int rawStringLength = (int)wcslen(rawString);
	fwrite(&rawStringLength, sizeof(rawStringLength), 1, file);
	fwrite(rawString, sizeof(WCHAR), rawStringLength, file);
	FreeRawString(rawString);
}

bool IsUniqueMeaning(const QuestionType* questionType, const Meaning* const oldMeanings[], int numberOfOldMeanings, const Meaning* meaning) {
	for (int i = 0; i < numberOfOldMeanings; ++i) {
		if (questionType->Type == GuessMeaning && (oldMeanings[i]->Word == meaning->Word || _tcscmp(oldMeanings[i]->Meaning, meaning->Meaning) == 0) ||
			questionType->Type == GuessWord && (oldMeanings[i]->Word == meaning->Word || _tcscmp(oldMeanings[i]->Meaning, meaning->Meaning) == 0) ||
			questionType->Type == GuessPronunciation && (oldMeanings[i]->Word == meaning->Word || _tcscmp(oldMeanings[i]->Pronunciation, meaning->Pronunciation) == 0)) return false;
	}
	return true;
}

void CreateQuestionOption(QuestionOption* questionOption) {
	CreateArray(&questionOption->Types, sizeof(QuestionType));
}

void GenerateQuestion(Question* question, const Meaning* answer) {
	const QuestionType* const prevQuestionType = question->Type;
	do {
		question->Type = (QuestionType*)GetElement(&question->Option->Types, rand() % question->Option->Types.Count);
	} while (question->Option->ExcludeDuplicatedAnswer && question->Type->UnusedVocabulary.Words.Count == 0);

	const Meaning* const oldAnswer = prevQuestionType > 0 ? question->Meanings[question->Answer] : NULL;
	if (question->Option->ExcludeDuplicatedAnswer) {
		do {
			Word* const word = GetWord(&question->Type->UnusedVocabulary, rand() % question->Type->UnusedVocabulary.Words.Count);
			answer = GetMeaning(word, rand() % word->Meanings.Count);
		} while (oldAnswer && !IsUniqueMeaning(prevQuestionType, &oldAnswer, 1, answer));
	}
	if (answer) {
		question->Meanings[0] = answer;
	}

	for (int i = answer != NULL; i < question->Option->NumberOfMeanings; ++i) {
		do {
			Word* const word = GetWord((Vocabulary*)&question->Option->Vocabulary, rand() % question->Option->Vocabulary.Words.Count);
			question->Meanings[i] = GetMeaning(word, rand() % word->Meanings.Count);
		} while (!IsUniqueMeaning(question->Type, question->Meanings, i, question->Meanings[i]));
	}

	if (answer) {
		question->Answer = rand() % question->Option->NumberOfMeanings;
		question->Meanings[0] = question->Meanings[question->Answer];
		question->Meanings[question->Answer] = answer;
	} else {
		do {
			question->Answer = rand() % question->Option->NumberOfMeanings;
		} while (oldAnswer && !IsUniqueMeaning(question->Type, &oldAnswer, 1, question->Meanings[question->Answer]));
	}
}