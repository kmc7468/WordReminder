#include "Word.h"

#include "String.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>

static LPTSTR ReadString(FILE* file);
static void WriteString(FILE* file, LPCTSTR string);

bool CopyWord(Word* dest, const Word* source) {
	bool success = true;
	success = success && (dest->Word = malloc(sizeof(TCHAR) * (_tcslen(source->Word) + 1)));
	success = success && (dest->Pronunciation = malloc(sizeof(TCHAR) * (_tcslen(source->Pronunciation) + 1)));
	success = success && (dest->Meaning = malloc(sizeof(TCHAR) * (_tcslen(source->Meaning) + 1)));
	if (!success) {
		DestroyWord(dest);
		return false;
	}

	_tcscpy(dest->Word, source->Word);
	_tcscpy(dest->Pronunciation, source->Pronunciation);
	_tcscpy(dest->Meaning, source->Meaning);
	dest->IsWrong = source->IsWrong;
	return true;
}
bool CompareWord(const Word* a, const Word* b) {
	return a == b || _tcscmp(a->Meaning, b->Meaning) == 0;
}
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
bool CopyVocabulary(Vocabulary* dest, const Vocabulary* source) {
	Word* const array = malloc(sizeof(Word) * source->Count);
	if (!array) return false;

	for (int i = 0; i < source->Count; ++i) {
		if (!CopyWord(array + i, source->Array + i)) {
			for (int j = 0; j < i; ++j) {
				DestroyWord(array + j);
			}
			free(array);
			return false;
		}
	}

	dest->Array = array;
	dest->Count = source->Count;
	dest->Capacity = source->Capacity;
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
void RemoveEqualWord(Vocabulary* vocabulary, const Word* word) {
	for (int i = 0; i < vocabulary->Count; ++i) {
		if (CompareWord(vocabulary->Array + i, word)) {
			DestroyWord(vocabulary->Array + i);
			memmove(vocabulary->Array + i, vocabulary->Array + i + 1, sizeof(Word) * (--vocabulary->Count - i));
			return;
		}
	}
}
Word* FindEqualWord(Vocabulary* vocabulary, const Word* word) {
	for (int i = 0; i < vocabulary->Count; ++i) {
		if (CompareWord(vocabulary->Array + i, word)) return vocabulary->Array + i;
	}
	return NULL;
}
int GetUniqueWordCount(const Vocabulary* vocabulary) {
	int result = vocabulary->Count;
	for (int i = 0; i < vocabulary->Count; ++i) {
		for (int j = 0; j < i; ++j) {
			if (CompareWord(vocabulary->Array + i, vocabulary->Array + j)) {
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

const QuestionType QuestionTypes[3] = { GuessMeaning, GuessWord, GuessPronunciation };

void GenerateQuestion(Question* question, QuestionOption* option, Word* answer, int selector, Vocabulary* unusedVocabularies) {
	int questionType;
	do {
		questionType = rand() % ARRAYSIZE(QuestionTypes);
		question->Type = QuestionTypes[questionType];
	} while ((question->Type & option->QuestionType) == 0 || unusedVocabularies && unusedVocabularies[questionType].Count == 0);

	const Word* const oldAnswer = question->Answer >= 0 ? question->Words[question->Answer] : NULL;
	if (unusedVocabularies) {
		do {
			answer = unusedVocabularies[questionType].Array + rand() % unusedVocabularies[questionType].Count;
		} while (oldAnswer && (CompareWord(oldAnswer, answer) ||
			question->Type == GuessPronunciation && _tcscmp(oldAnswer->Pronunciation, answer->Pronunciation) == 0));
		answer = FindEqualWord(&option->Vocabulary, answer);
	}
	if (answer) {
		question->Words[0] = answer;
	}

	for (int i = answer != NULL; i < selector; ++i) {
		bool unique = false;
		do {
			question->Words[i] = option->Vocabulary.Array + rand() % option->Vocabulary.Count;
			for (int j = 0; j <= i; ++j) {
				if (i == j) {
					unique = true;
				} else if (CompareWord(question->Words[i], question->Words[j]) ||
					question->Type == GuessPronunciation && _tcscmp(question->Words[i]->Pronunciation, question->Words[j]->Pronunciation) == 0) break;
			}
		} while (!unique);
	}

	if (answer) {
		question->Answer = rand() % selector;
		question->Words[0] = question->Words[question->Answer];
		question->Words[question->Answer] = answer;
	} else {
		do {
			question->Answer = rand() % selector;
		} while (oldAnswer && CompareWord(oldAnswer, question->Words[question->Answer]));
	}
}
void SetSelectorText(const Question* question, const QuestionOption* option, HWND* buttons, int selector, bool mustSetWord) {
	for (int i = 0; i < selector; ++i) {
		if (mustSetWord || question->Type == GuessWord) {
			if (option->GivePronunciation && question->Words[i]->Pronunciation[0] != 0 &&
				_tcscmp(question->Words[i]->Word, question->Words[i]->Pronunciation)) {
				LPTSTR text = malloc(sizeof(TCHAR) * (_tcslen(question->Words[i]->Word) + _tcslen(question->Words[i]->Pronunciation) + 4));
				_tcscpy(text, question->Words[i]->Word);
				_tcscat(text, _T("\n("));
				_tcscat(text, question->Words[i]->Pronunciation);
				_tcscat(text, _T(")"));

				SetWindowText(buttons[i], text);
				free(text);
			} else {
				SetWindowText(buttons[i], question->Words[i]->Word);
			}
		} else if (question->Type == GuessMeaning) {
			SetWindowText(buttons[i], question->Words[i]->Meaning);
		} else {
			SetWindowText(buttons[i], question->Words[i]->Pronunciation);
		}

		EnableWindow(buttons[i], TRUE);
	}
}

LPTSTR ReadString(FILE* file) {
	int length;
	fread(&length, sizeof(length), 1, file);
	const LPWSTR rawStr = malloc(sizeof(WCHAR) * (length + 1));
	fread(rawStr, sizeof(*rawStr), length, file);
	rawStr[length] = 0;
	return MakeUniString(rawStr);
}
void WriteString(FILE* file, LPCTSTR string) {
	const LPCWSTR raw = GetRawString(string);
	const int rawLength = (int)wcslen(raw);
	fwrite(&rawLength, sizeof(rawLength), 1, file);
	fwrite(raw, sizeof(WCHAR), rawLength, file);
	FreeRawString(raw);
}