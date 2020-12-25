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

bool CopyWord(Word* dest, const Word* source);
bool CompareWord(const Word* a, const Word* b);
void DestroyWord(Word* word);

typedef struct {
	Word* Array;
	int Count;
	int Capacity;
} Vocabulary;

bool LoadVocabulary(Vocabulary* vocabulary, LPCTSTR path);
bool SaveVocabulary(const Vocabulary* vocabulary, LPCTSTR path);
bool CopyVocabulary(Vocabulary* dest, const Vocabulary* source);
bool AddWord(Vocabulary* vocabulary, const Word* word);
void RemoveWord(Vocabulary* vocabulary, int index);
void RemoveEqualWord(Vocabulary* vocabulary, const Word* word);
Word* FindEqualWord(Vocabulary* vocabulary, const Word* word);
int GetUniqueWordCount(const Vocabulary* vocabulary);
void DestroyVocabulary(Vocabulary* vocabulary);

typedef enum {
	GuessMeaning = 1,
	GuessWord = 2,
	GuessPronunciation = 4,
} QuestionType;

extern const QuestionType QuestionTypes[3];

typedef struct {
	QuestionType Type;
	Word* Words[5];
	int Answer;
} Question;

typedef struct {
	Vocabulary Vocabulary;
	QuestionType QuestionType;
	bool GivePronunciation;
	bool ExcludeDuplicatedAnswer;
} QuestionOption;

void GenerateQuestion(Question* question, QuestionOption* option, Word* answer, int selector, Vocabulary* unusedVocabularies);
void SetSelectorText(const Question* question, const QuestionOption* option, HWND* buttons, int selector, bool mustSetWord);