#pragma once

#include "Array.h"
#include "WinAPI.h"

typedef enum {
	Constant,
	DependentOnWidth,
	DependentOnHeight,
	DependentOnChildren,
} UILengthType;

typedef struct {
	UILengthType Type;
	int Constant;
	Array Terms;

	float Evaluated;
} UILength;

void CreateUILength(UILength* uiLength);
void DestroyUILength(UILength* uiLength);
void AddTerm(UILength* uiLength, UILength* term);
void EvaluateUILength(UILength* uiLength, HWND window, float width, float height);

typedef enum {
	Window,
	Horizontal,
	Vertical,
} UIComponentType;

typedef enum {
	None,
	Center,
	CenterWithMargin,
	Bottom,
	Right = Bottom,
} UIComponentAlignment;

typedef struct {
	LPTSTR Name;
	UIComponentType Type;
	UIComponentAlignment Alignment;
	UILength* Length;
	Array Children;

	HWND* Window;
	float EvaluatedX;
	float EvaluatedY;
	float EvaluatedWidth;
	float EvaluatedHeight;
} UIComponent;

void CreateUIComponent(UIComponent* uiComponent, LPCTSTR name);
void DestroyUIComponent(UIComponent* uiComponent);
void AddChild(UIComponent* uiComponent, UIComponent* child);
UIComponent* FindUIComponent(UIComponent* uiComponent, LPCTSTR name);
void ApplyUIComponent(UIComponent* uiComponent, HWND window);
void EvaluateUIComponent(UIComponent* uiComponent, HWND window, float x, float y, float width, float height);
void UpdateUIComponent(UIComponent* uiComponent);

void CreateUIEngine(UIComponent* uiEngine);
void EvaluateUIEngine(UIComponent* uiEngine, HWND window, int width, int height);
void UpdateUIEngine(UIComponent* uiEngine);
void DestroyUIEngine(UIComponent* uiEngine);

int GetCenterX(const UIComponent* uiComponent);
int GetX(const UIComponent* uiComponent);
int GetY(const UIComponent* uiComponent);

#define UICOMP_BASE(tag, name, window, type, alignment, lengthType, constant, parent)		\
UIComponent* tag = calloc(1, sizeof(UIComponent));											\
CreateUIComponent(tag, name);																\
tag->Type = type;																			\
tag->Alignment = alignment;																	\
tag->Length = calloc(1, sizeof(UILength));													\
CreateUILength(tag->Length);																\
tag->Length->Type = lengthType;																\
tag->Length->Constant = constant;															\
tag->Window = window;																		\
AddChild(parent, tag)
#define UICOMP_CON(tag, type, alignment, constant, parent)				UICOMP_BASE(tag, NULL, NULL, type, alignment, Constant, constant, parent)
#define UICOMP_CON_N(tag, name, type, alignment, constant, parent)		UICOMP_BASE(tag, name, NULL, type, alignment, Constant, constant, parent)
#define UICOMP_CON_W(tag, window, type, alignment, constant, parent)	UICOMP_BASE(tag, NULL, window, type, alignment, Constant, constant, parent)
#define UICOMP_DOW(tag, type, alignment, constant, parent)				UICOMP_BASE(tag, NULL, NULL, type, alignment, DependentOnWidth, constant, parent)
#define UICOMP_DOW_N(tag, name, type, alignment, constant, parent)		UICOMP_BASE(tag, name, NULL, type, alignment, DependentOnWidth, constant, parent)
#define UICOMP_DOW_W(tag, window, type, alignment, constant, parent)	UICOMP_BASE(tag, NULL, window, type, alignment, DependentOnWidth, constant, parent)
#define UICOMP_DOH(tag, type, alignment, constant, parent)				UICOMP_BASE(tag, NULL, NULL, type, alignment, DependentOnHeight, constant, parent)
#define UICOMP_DOH_N(tag, name, type, alignment, constant, parent)		UICOMP_BASE(tag, name, NULL, type, alignment, DependentOnHeight, constant, parent)
#define UICOMP_DOH_W(tag, window, type, alignment, constant, parent)	UICOMP_BASE(tag, NULL, window, type, alignment, DependentOnHeight, constant, parent)
#define UICOMP_DOC(tag, type, alignment, parent)						UICOMP_BASE(tag, NULL, NULL, type, alignment, DependentOnChildren, 0, parent)
#define UICOMP_DOC_N(tag, name, type, alignment, parent)				UICOMP_BASE(tag, name, NULL, type, alignment, DependentOnChildren, 0, parent)
#define UICOMP_DOC_W(tag, window, type, alignment, parent)				UICOMP_BASE(tag, NULL, window, type, alignment, DependentOnChildren, 0, parent)

#define UICOMP_FIND(tag, name) UIComponent* const tag = FindUIComponent(&g_UIEngine, name)