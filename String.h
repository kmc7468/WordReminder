#pragma once

#include <Windows.h>

LPTSTR MakeGenericString(LPWSTR rawString);
LPCWSTR GetRawString(LPCTSTR genericString);
void FreeRawString(LPCWSTR rawString);