#include "Array.h"

#include <stdlib.h>
#include <string.h>
#include <Windows.h>

void CreateArray(Array* array, int elementSize) {
	memset(array, 0, sizeof(*array));

	array->ElementSize = elementSize;
}
void AddElement(Array* array, void* data) {
	if (array->Count == array->Capacity) {
		array->Capacity = max(1, array->Capacity * 2);
		array->Array = realloc(array->Array, array->ElementSize * array->Capacity);
	}

	memcpy(GetElement(array, array->Count++), data, array->ElementSize);
}
void RemoveElement(Array* array, int index) {
	memmove(GetElement(array, index), GetElement(array, index + 1), array->ElementSize * (--array->Count - index));
}
void* GetElement(Array* array, int index) {
	return (LPBYTE)array + array->ElementSize * index;
}
int FindElement(Array* array, void* data, bool(compareFunction)(void*, void*)) {
	for (int i = 0; i < array->Count; ++i) {
		if (compareFunction(GetElement(array, i), data)) return i;
	}
	return -1;
}
void DestroyArray(Array* array) {
	free(array->Array);
}