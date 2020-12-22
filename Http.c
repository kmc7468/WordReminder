#include "Http.h"

#include "String.h"

#include <stdlib.h>
#include <tchar.h>

bool CreateHttpRequest(HttpRequest* request, LPCTSTR url, LPCTSTR method, bool https) {
	const LPCWSTR rawUrl = GetRawString(url);
	const int rawUrlLength = (int)wcslen(rawUrl);

	URL_COMPONENTS comp = { 0 };
	comp.dwHostNameLength = (DWORD)-1;
	comp.dwStructSize = sizeof(comp);
	comp.dwUrlPathLength = (DWORD)-1;

	const bool success = WinHttpCrackUrl(rawUrl, rawUrlLength, 0, &comp);
	FreeRawString(rawUrl);
	if (!success) return false;

	const HINTERNET session = WinHttpOpen(L"WordReminder",
		WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
	if (!session) return false;

	const LPWSTR urlPath = calloc(comp.dwHostNameLength + 1, sizeof(WCHAR));
	_tcsncpy(urlPath, comp.lpszHostName, comp.dwHostNameLength);

	const HINTERNET connect = WinHttpConnect(session, urlPath, comp.nPort, 0);
	free(urlPath);
	if (!connect) {
		WinHttpCloseHandle(session);
		return false;
	}

	const LPCWSTR rawMethod = GetRawString(method);
	const HINTERNET request2 = WinHttpOpenRequest(connect, rawMethod, comp.lpszUrlPath, NULL,
		WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, https ? WINHTTP_FLAG_SECURE : 0);
	FreeRawString(rawMethod);
	if (!request2) {
		WinHttpCloseHandle(connect);
		WinHttpCloseHandle(session);
		return false;
	}

	DWORD buffer = WINHTTP_DISABLE_REDIRECTS;
	WinHttpSetOption(request2, WINHTTP_OPTION_DISABLE_FEATURE, &buffer, sizeof(buffer));

	request->Session = session;
	request->Connect = connect;
	request->Request = request2;
	return true;
}
bool SendHttpRequest(HttpRequest* request, LPCTSTR header) {
	LPCWSTR rawHeader = NULL;
	int rawHeaderLength = 0;
	if (header) {
		rawHeader = GetRawString(rawHeader);
		rawHeaderLength = (int)wcslen(rawHeader);
	}

	const bool success = WinHttpSendRequest(request->Request, rawHeader, rawHeaderLength, NULL, 0, 0, 0);
	if (rawHeader) {
		FreeRawString(rawHeader);
	}
	return success && WinHttpReceiveResponse(request->Request, NULL);
}
LPTSTR GetHttpResponseHeader(HttpRequest* request, HttpResponseHeader header) {
	DWORD bufferLength = 0;
	WinHttpQueryHeaders(request->Request, (DWORD)header, NULL, WINHTTP_NO_OUTPUT_BUFFER, &bufferLength, NULL);
	if (GetLastError() != ERROR_INSUFFICIENT_BUFFER) return NULL;

	LPWSTR buffer = malloc(bufferLength);
	WinHttpQueryHeaders(request->Request, (DWORD)header, NULL, buffer, &bufferLength, NULL);
	return MakeUniString(buffer);
}
HttpResponseBody GetHttpResponseBody(HttpRequest* request) {
	HttpResponseBody result = { 0 };

	if (!WinHttpQueryDataAvailable(request->Request, &result.Length)) return result;

	result.Data = malloc(result.Length);
	WinHttpReadData(request->Request, result.Data, result.Length, &result.Length);
	return result;
}
void DestroyHttpRequest(HttpRequest* request) {
	WinHttpCloseHandle(request->Request);
	WinHttpCloseHandle(request->Connect);
	WinHttpCloseHandle(request->Session);
}