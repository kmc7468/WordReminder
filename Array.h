#pragma once

#include <stdbool.h>

typedef struct {
	void* Array;
	int ElementSize;
	int Count;
	int Capacity;
} Array;

void CreateArray(Array* array, int elementSize);
void AddElement(Array* array, void* data);
void RemoveElement(Array* array, int index);
void* GetElement(Array* array, int index);
int FindElement(Array* array, void* data, bool(compareFunction)(void*, void*));
void DestroyArray(Array* array);