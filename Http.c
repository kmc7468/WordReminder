#include "Http.h"

#include "String.h"

#include <string.h>

bool CreateHttpRequest(HttpRequest* httpRequest, LPCTSTR url, LPCTSTR method, bool enableSsl) {
	httpRequest->Session = WinHttpOpen(L"WordReminder",
		WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
	if (!httpRequest->Session) goto Destroy;

	const LPCWSTR rawUrl = GetRawString(url);
	const int rawUrlLength = (int)wcslen(rawUrl);

	URL_COMPONENTS comp = { 0 };
	comp.dwHostNameLength = (DWORD)-1;
	comp.dwStructSize = sizeof(comp);
	comp.dwUrlPathLength = (DWORD)-1;

	const bool success = WinHttpCrackUrl(rawUrl, rawUrlLength, 0, &comp);
	FreeRawString(rawUrl);
	if (!success) goto Destroy;

	const LPWSTR serverName = calloc(comp.dwHostNameLength + 1, sizeof(WCHAR));
	wcsncpy(serverName, comp.lpszHostName, comp.dwHostNameLength);

	httpRequest->Connect = WinHttpConnect(httpRequest->Session, serverName, comp.nPort, 0);
	free(serverName);
	if (!httpRequest->Connect) goto Destroy;

	const LPCWSTR rawMethod = GetRawString(method);
	httpRequest->Request = WinHttpOpenRequest(httpRequest->Connect, rawMethod, comp.lpszUrlPath,
		NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, enableSsl ? WINHTTP_FLAG_SECURE : 0);
	FreeRawString(rawMethod);
	if (!httpRequest->Request) goto Destroy;

	DWORD option = WINHTTP_DISABLE_REDIRECTS;
	WinHttpSetOption(httpRequest->Request, WINHTTP_OPTION_DISABLE_FEATURE, &option, sizeof(option));
	return true;

Destroy:
	DestroyHttpRequest(httpRequest);
	return false;
}
bool SendHttpRequest(HttpRequest* httpRequest, LPCTSTR headers) {
	const LPCWSTR rawHeaders = headers ? GetRawString(headers) : NULL;
	const int rawHeadersLength = headers ? (int)wcslen(rawHeaders) : 0;

	const bool success = WinHttpSendRequest(httpRequest->Request, rawHeaders, rawHeadersLength, NULL, 0, 0, 0);
	FreeRawString(rawHeaders);
	return success && (bool)WinHttpReceiveResponse(httpRequest->Request, NULL);
}
LPCTSTR GetHttpResponseHeader(HttpRequest* httpRequest, HttpResponseHeader header) {
	DWORD length = 0;
	WinHttpQueryHeaders(httpRequest->Request, (DWORD)header, NULL, WINHTTP_NO_OUTPUT_BUFFER, &length, NULL);
	if (GetLastError() != ERROR_INSUFFICIENT_BUFFER) return NULL;

	const LPWSTR result = malloc(length);
	WinHttpQueryHeaders(httpRequest->Request, (DWORD)header, NULL, result, &length, NULL);
	return MakeGenericString(result);
}
HttpResponseBody GetHttpResponseBody(HttpRequest* httpRequest) {
	HttpResponseBody result = { 0 };
	if (!WinHttpQueryDataAvailable(httpRequest->Request, &result.Length)) return result;

	result.Buffer = malloc(result.Length);
	WinHttpReadData(httpRequest->Request, result.Buffer, result.Length, &result.Length);
	return result;
}
void DestroyHttpRequest(HttpRequest* httpRequest) {
	WinHttpCloseHandle(httpRequest->Request);
	WinHttpCloseHandle(httpRequest->Connect);
	WinHttpCloseHandle(httpRequest->Session);
}