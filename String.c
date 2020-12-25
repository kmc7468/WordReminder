#include "String.h"

#include <stdbool.h>
#include <stdlib.h>

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