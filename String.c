#include "String.h"

#include <stdbool.h>
#include <stdlib.h>

static LPSTR WCS2MBS(LPCWSTR wcs, bool destroyWCS);
static LPWSTR MBS2WCS(LPCSTR mbs, bool destroyMBS);

LPTSTR MakeUniString(LPWSTR raw) {
#if defined(UNICODE) || defined(_UNICODE)
	return raw;
#else
	return WCS2MBS(raw, true);
#endif
}
LPCWSTR GetRawString(LPCTSTR tcs) {
#if defined(UNICODE) || defined(_UNICODE)
	return tcs;
#else
	return MBS2WCS(tcs, false);
#endif
}
void FreeRawString(LPCWSTR raw) {
#if defined(UNICODE) || defined(_UNICODE)
	(void)raw;
#else
	free((LPWSTR)raw);
#endif
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