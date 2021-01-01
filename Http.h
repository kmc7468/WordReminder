#pragma once

#include "WinAPI.h"

#include <stdbool.h>

typedef struct {
	HINTERNET Session, Connect, Request;
} HttpRequest;

typedef enum {
	Location = WINHTTP_QUERY_LOCATION,
} HttpResponseHeader;

typedef struct {
	LPBYTE Buffer;
	DWORD Length;
} HttpResponseBody;

bool CreateHttpRequest(HttpRequest* httpRequest, LPCTSTR url, LPCTSTR method, bool enableSsl);
bool SendHttpRequest(HttpRequest* httpRequest, LPCTSTR headers);
LPTSTR GetHttpResponseHeader(HttpRequest* httpRequest, HttpResponseHeader header);
HttpResponseBody GetHttpResponseBody(HttpRequest* httpRequest);
void DestroyHttpRequest(HttpRequest* httpRequest);