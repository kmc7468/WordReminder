#pragma once

#include "Array.h"
#include "WinAPI.h"

typedef enum {
	Constant,
	DependentOnWidth,
	DependentOnHeight,
	DependentOnChildren,
	Sum,
} UILengthType;

typedef struct {
	UILengthType Type;
	int Constant;
	Array Terms;

	int Evaluated;
} UILength;

void CreateUILength(UILength* uiLength);
void DestroyUILength(UILength* uiLength);
void AddTerm(UILength* uiLength, UILength* term);
void EvaluateUILength(UILength* uiLength, HWND window, int width, int height);

typedef enum {
	Window,
	Horizontal,
	Vertical,
} UIComponentType;

typedef enum {
	None,
	Center,
} UIComponentAlignment;

typedef struct {
	LPTSTR Name;
	UIComponentType Type;
	UIComponentAlignment Alignment;
	UILength* Length;
	Array Children;

	int EvaluatedX;
	int EvaluatedY;
	int EvaluatedWidth;
	int EvaluatedHeight;
} UIComponent;

void CreateUIComponent(UIComponent* uiComponent, LPCTSTR name);
void DestroyUIComponent(UIComponent* uiComponent);
void AddChild(UIComponent* uiComponent, UIComponent* child);
UIComponent* FindUIComponent(UIComponent* uiComponent, LPCTSTR name);
void ApplyUIComponent(UIComponent* uiComponent, HWND window);
void EvaluateUIComponent(UIComponent* uiComponent, HWND window, int x, int y, int width, int height);

void CreateUIEngine(UIComponent* uiEngine);
void EvaluateUIEngine(UIComponent* uiEngine, HWND window, int width, int height);
void DestroyUIEngine(UIComponent* uiEngine);

int GetCenterX(const UIComponent* uiComponent);

#define UICOMP_BASE(tag, name, type, alignment, lengthType, constant, parent)		\
UIComponent* tag = calloc(1, sizeof(UIComponent));									\
CreateUIComponent(tag, name);														\
tag->Type = type;																	\
tag->Alignment = alignment;															\
tag->Length = calloc(1, sizeof(UILength));											\
CreateUILength(tag->Length);														\
tag->Length->Type = lengthType;														\
tag->Length->Constant = constant;													\
AddChild(parent, tag)
#define UICOMP_CON(tag, name, type, alignment, constant, parent) UICOMP_BASE(tag, name, type, alignment, Constant, constant, parent)
#define UICOMP_DOW(tag, name, type, alignment, percent, parent) UICOMP_BASE(tag, name, type, alignment, DependentOnWidth, percent, parent)
#define UICOMP_DOH(tag, name, type, alignment, percent, parent) UICOMP_BASE(tag, name, type, alignment, DependentOnHeight, percent, parent)
#define UICOMP_DOC(tag, name, type, alignment, parent) UICOMP_BASE(tag, name, type, alignment, DependentOnChildren, 0, parent)

#define UICOMP_FIND(tag, name) UIComponent* const tag = FindUIComponent(&g_UIEngine, name)