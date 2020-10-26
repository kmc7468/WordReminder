#pragma once

#include <stdbool.h>
#include <Windows.h>

typedef struct {
	LPTSTR Word;
	LPTSTR Pronunciation;
	LPTSTR Meaning;
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
int GetUniqueWordCount(const Vocabulary* vocabulary);
void DestroyVocabulary(Vocabulary* vocabulary);