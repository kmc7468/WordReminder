#pragma once

#include "Array.h"
#include "WinAPI.h"

typedef struct {
	LPTSTR Word;
	Array Meanings;
} Word;

void CreateWord(Word* word);
void CopyWord(Word* destination, const Word* source);
int FindMeaning(const Word* word, LPCTSTR meaning);
void DestroyWord(Word* word);

typedef struct {
	int Word;
	LPTSTR Pronunciation;
	LPTSTR Meaning;
	int Wrong;
} Meaning;

void CopyMeaning(Meaning* destination, const Meaning* source);
void DestroyMeaning(Meaning* meaning);

void AddMeaning(Word* word, Meaning* meaning);
void RemoveMeaning(Word* word, int index);
Meaning* GetMeaning(Word* word, int index);

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
int FindWord(const Vocabulary* vocabulary, LPCTSTR word);
void DestroyVocabulary(Vocabulary* vocabulary, bool destroyWords);

typedef enum {
	GuessMeaning,
	GuessWord,
	GuessPronunciation,
} QuestionTypeType;

typedef struct {
	QuestionTypeType Type;
	Vocabulary UnusedVocabulary;

	int Option;
} QuestionType;

void CreateQuestionType(QuestionType* questionType);
void DestroyQuestionType(QuestionType* questionType);
bool IsUniqueMeaning(const QuestionType* questionType, const Meaning* const oldMeanings[], int numberOfOldMeanings, const Meaning* meaning);

typedef struct {
	Vocabulary Vocabulary;
	Array Types;
	int NumberOfSelectors;

	bool ExcludeDuplicatedAnswer;
} QuestionOption;

void CreateQuestionOption(QuestionOption* questionOption);
void DestroyQuestionOption(QuestionOption* questionOption);

typedef struct {
	QuestionOption* Option;
	QuestionType* Type;
	const Meaning* Meanings[5];
	int Answer;
} Question;

void GenerateQuestion(Question* question, const Meaning* answer);