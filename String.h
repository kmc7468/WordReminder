#pragma once

#include "Array.h"
#include "WinAPI.h"

LPTSTR MakeGenericString(LPWSTR rawString);
void FreeGenericString(LPCTSTR genericString);
LPCWSTR GetRawString(LPCTSTR genericString);
void FreeRawString(LPCWSTR rawString);

LPSTR EncodeToUTF8(LPTSTR genericString);

typedef struct {
	int Major;
	int Minor;
	int Patch;
	Array Identifiers;
} SemanticVersion;

void CreateSemanticVersion(SemanticVersion* semanticVersion);
void ParseSemanticVersion(SemanticVersion* semanticVersion, LPTSTR semanticVersionString);
int CompareSemanticVersion(const SemanticVersion* a, const SemanticVersion* b);
void DestroySemanticVersion(SemanticVersion* semanticVersion);