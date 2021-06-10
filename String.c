#include "String.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <wctype.h>

LPTSTR MakeGenericString(LPWSTR rawString) {
#ifdef _UNICODE
	return rawString;
#else
	const int length = WideCharToMultiByte(CP_ACP, 0, rawString, -1, NULL, 0, NULL, NULL);
	const LPTSTR result = malloc(sizeof(TCHAR) * length);
	WideCharToMultiByte(CP_ACP, 0, rawString, -1, result, length, NULL, NULL);

	free(rawString);
	return result;
#endif
}
void FreeGenericString(LPCTSTR genericString) {
#ifdef _UNICODE
	(void)genericString;
#else
	free((LPTSTR)genericString);
#endif
}
LPCWSTR GetRawString(LPCTSTR genericString) {
#ifdef _UNICODE
	return genericString;
#else
	const int length = MultiByteToWideChar(CP_ACP, 0, genericString, -1, NULL, 0);
	const LPWSTR result = malloc(sizeof(WCHAR) * length);
	MultiByteToWideChar(CP_ACP, 0, genericString, -1, result, length);
	return result;
#endif
}
void FreeRawString(LPCWSTR rawString) {
#ifdef _UNICODE
	(void)rawString;
#else
	free((LPWSTR)rawString);
#endif
}

LPSTR EncodeToUTF8(LPTSTR genericString) {
	const LPWSTR rawString = GetRawString(genericString);
	const int length = WideCharToMultiByte(CP_UTF8, 0, rawString, -1, NULL, 0, NULL, NULL);

	LPSTR result = malloc(length);
	WideCharToMultiByte(CP_UTF8, 0, rawString, -1, result, length, NULL, NULL);

	FreeRawString(rawString);
	return result;
}

static bool IsIntegerIdentifier(LPTSTR identifier);

void CreateSemanticVersion(SemanticVersion* semanticVersion) {
	CreateArray(&semanticVersion->Identifiers, sizeof(LPTSTR));
}
void ParseSemanticVersion(SemanticVersion* semanticVersion, LPTSTR semanticVersionString) {
	int* const number[] = { &semanticVersion->Major, &semanticVersion->Minor, &semanticVersion->Patch };
	int* const* currentNumber = number;

	LPTSTR numberBegin = semanticVersionString, identiferBegin = NULL;
	bool loop = true;
	while (loop) {
		if (identiferBegin) {
			if (!_istalpha(*semanticVersionString) && !_istdigit(*semanticVersionString)) {
				if (*semanticVersionString == 0) {
					loop = false;
				}

				*semanticVersionString = 0;
				AddElement(&semanticVersion->Identifiers, &identiferBegin);
				identiferBegin = semanticVersionString + 1;
			}
		} else {
			if (!_istdigit(*semanticVersionString)) {
				if (*semanticVersionString == 0) {
					loop = false;
				} else if (*semanticVersionString == '-') {
					identiferBegin = semanticVersionString + 1;
				}

				*semanticVersionString = 0;
				**currentNumber++ = _ttoi(numberBegin);
				numberBegin = semanticVersionString + 1;
			}
		}

		++semanticVersionString;
	}
}
int CompareSemanticVersion(const SemanticVersion* a, const SemanticVersion* b) {
	const int* const aNumber[] = { &a->Major, &a->Minor, &a->Patch };
	const int* const bNumber[] = { &b->Major, &b->Minor, &b->Patch };
	for (int i = 0; i < ARRAYSIZE(aNumber); ++i) {
		if (*aNumber[i] > *bNumber[i]) return 1;
		else if (*aNumber[i] < *bNumber[i]) return -1;
	}

	if (a->Identifiers.Count == 0 && b->Identifiers.Count == 0) return 0;
	else if (a->Identifiers.Count == 0 && b->Identifiers.Count > 0) return 1;
	else if (a->Identifiers.Count > 0 && b->Identifiers.Count == 0) return -1;

	for (int i = 0; i < min(a->Identifiers.Count, b->Identifiers.Count); ++i) {
		const LPTSTR aIdentifer = *(LPTSTR*)GetElement((Array*)&a->Identifiers, i);
		const LPTSTR bIdentifer = *(LPTSTR*)GetElement((Array*)&b->Identifiers, i);

		const bool isAIntegerDigit = IsIntegerIdentifier(aIdentifer);
		const bool isBIntegerDigit = IsIntegerIdentifier(bIdentifer);
		if (isAIntegerDigit && isBIntegerDigit) {
			const int aInteger = _ttoi(aIdentifer);
			const int bInteger = _ttoi(bIdentifer);
			if (aInteger > bInteger) return 1;
			else if (aInteger < bInteger) return -1;
		} else if (isAIntegerDigit && !isBIntegerDigit) return -1;
		else if (!isAIntegerDigit && isBIntegerDigit) return 1;
		else {
			int result = _tcscmp(aIdentifer, bIdentifer);
			result = (result > 0) - (result < 0);
			if (result) return result;
		}
	}

	if (a->Identifiers.Count > b->Identifiers.Count) return 1;
	else if (a->Identifiers.Count < b->Identifiers.Capacity) return -1;
	else return 0;
}
void DestroySemanticVersion(SemanticVersion* semanticVersion) {
	DestroyArray(&semanticVersion->Identifiers);
}

bool IsIntegerIdentifier(LPTSTR identifier) {
	while (*identifier) {
		if (!_istdigit(*identifier++)) return false;
	}
	return true;
}