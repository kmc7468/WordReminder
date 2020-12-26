#pragma once

#include "Array.h"

#include <Windows.h>

typedef struct {
	LPTSTR Word;
	Array Meanings;
} Word;

typedef struct {
	Word* Word;
	LPTSTR Pronunciation;
	LPTSTR Meaning;
	int Wrong;
} Meaning;

void CopyMeaning(Meaning* destination, const Meaning* source);
void DestroyMeaning(Meaning* meaning);

void CreateWord(Word* word);
void CopyWord(Word* destination, const Word* source);
void AddMeaning(Word* word, Meaning* meaning);
void RemoveMeaning(Word* word, int index);
Meaning* GetMeaning(Word* word, int index);
void DestroyWord(Word* word);

typedef struct {
	Array Words;
} Vocabulary;

void CreateVocabulary(Vocabulary* vocabulary);
void CopyVocabulary(Vocabulary* destination, const Vocabulary* source);
bool LoadVocabulary(Vocabulary* vocabulary, LPCTSTR path);
bool SaveVocabulary(const Vocabulary* vocabulary, LPCTSTR path);
void AddWord(Vocabulary* vocabulary, Word* word);
void RemoveWord(Vocabulary* vocabulary, int index);
Word* GetWord(Vocabulary* vocabulary, int index);
void DestroyVocabulary(Vocabulary* vocabulary);

typedef enum {
	GuessMeaning = 1,
	GuessWord = 2,
	GuessPronunciation = 4,
} QuestionType;

extern const QuestionType QuestionTypes[3];

bool IsUniqueMeaning(QuestionType questionType, const Meaning* const oldMeanings[], int numberOfOldMeanings, const Meaning* meaning);

typedef struct {
	QuestionType Type;
	const Meaning* Meanings[5];
	int Answer;
} Question;

typedef struct {
	Vocabulary Vocabulary;
	QuestionType QuestionType;
	int NumberOfMeanings;

	bool ShowPronunciation;
	bool ExcludeDuplicatedAnswer;
} QuestionOption;

void GenerateQuestion(Question* question, const QuestionOption* questionOption, const Meaning* answer, const Vocabulary unusedVocabularies[]);