#pragma once

#include <stdbool.h>
#include <Windows.h>
#include <winhttp.h>

typedef struct {
	HINTERNET Session, Connect, Request;
} HttpRequest;

typedef enum {
	HeaderLocation = WINHTTP_QUERY_LOCATION,
} HttpResponseHeader;

typedef struct {
	LPBYTE Data;
	DWORD Length;
} HttpResponseBody;

bool CreateHttpRequest(HttpRequest* request, LPCTSTR url, LPCTSTR method, bool https);
bool SendHttpRequest(HttpRequest* request, LPCTSTR header);
LPTSTR GetHttpResponseHeader(HttpRequest* request, HttpResponseHeader header);
HttpResponseBody GetHttpResponseBody(HttpRequest* request);
void DestroyHttpRequest(HttpRequest* request);