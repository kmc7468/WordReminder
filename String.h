#pragma once

#include <Windows.h>

LPTSTR MakeUniString(LPWSTR raw);
LPCWSTR GetRawString(LPCTSTR tcs);
void FreeRawString(LPCWSTR raw);