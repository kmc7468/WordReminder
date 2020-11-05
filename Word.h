#pragma once

#include <stdbool.h>
#include <WinSock2.h>
#include <Windows.h>

typedef struct {
	LPTSTR Word;
	LPTSTR Pronunciation;
	LPTSTR Meaning;
	bool IsWrong;
} Word;

void DestroyWord(Word* word);

typedef struct {
	Word* Array;
	int Count;
	int Capacity;
} Vocabulary;

bool LoadVocabulary(Vocabulary* vocabulary, LPCTSTR path);
bool SaveVocabulary(const Vocabulary* vocabulary, LPCTSTR path);
bool AddWord(Vocabulary* vocabulary, const Word* word);
void RemoveWord(Vocabulary* vocabulary, int index);
void CopyWord(Word* dest, const Word* source);
int GetUniqueWordCount(const Vocabulary* vocabulary);
void DestroyVocabulary(Vocabulary* vocabulary);

typedef enum {
	GuessingMeaning = 1,
	GuessingWord = 2,
	GuessingBoth = GuessingMeaning | GuessingWord,
} QuestionType;

typedef struct {
	QuestionType Type;
	Word* Words[5];
	int Answer;
} Question;

typedef struct {
	Vocabulary Vocabulary;
	QuestionType QuestionType;
	bool ShouldGivePronunciation;
} QuestionOption;

void GenerateQuestion(Question* question, const QuestionOption* option);