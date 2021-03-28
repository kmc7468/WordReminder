#pragma once

#include "Array.h"
#include "WinAPI.h"

#include <stdbool.h>

typedef enum {
	Constant,
	FitConstant,
	DependentOnWidth,
	DependentOnHeight,
	DependentOnChildren,
} UILengthType;

typedef struct {
	UILengthType Type;
	float Constant;

	float Evaluated;
} UILength;

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
	UILength* Height;
	bool IsBold;

	HFONT Font;
	int EvaluatedHeight;
	bool ShouldCreate;
} UIComponentFont;

#define UIFONT_BASE(tag, lengthType, constant, isBold)											\
UIComponentFont* tag = calloc(1, sizeof(UIComponentFont));										\
tag->Height = calloc(1, sizeof(UILength));														\
tag->Height->Type = lengthType;																	\
tag->Height->Constant = constant;																\
tag->IsBold = isBold;																			\
AddFont(uiEngine, tag)
#define UIFONT_CON(tag, constant, isBold)									UIFONT_BASE(tag, Constant, constant, isBold)
#define UIFONT_FCN(tag, constant, isBold)									UIFONT_BASE(tag, FitConstant, constant, isBold)
#define UIFONT_DOW(tag, constant, isBold)									UIFONT_BASE(tag, DependentOnWidth, constant, isBold)
#define UIFONT_DOH(tag, constant, isBold)									UIFONT_BASE(tag, DependentOnHeight, constant, isBold)

void EvaluateUIComponentFont(UIComponentFont* uiComponentFont, HWND window, int width, int height);
void UpdateUIComponentFont(UIComponentFont* uiComponentFont);
void DestroyUIComponentFont(UIComponentFont* uiComponentFont);

typedef struct {
	LPTSTR Name;
	UIComponentType Type;
	UIComponentAlignment Alignment;
	UILength* Length;
	Array Children;

	UILength* LeftMargin;
	UILength* RightMargin;
	UILength* TopMargin;
	UILength* BottomMargin;
	bool IsOverridable;

	UIComponentFont* Font;

	HWND* Window;
	float EvaluatedX;
	float EvaluatedY;
	float EvaluatedWidth;
	float EvaluatedHeight;
	float EvaluatedVirtualWidth;
	float EvaluatedVirtualHeight;
} UIComponent;

void CreateUIComponent(UIComponent* uiComponent, LPCTSTR name);
void DestroyUIComponent(UIComponent* uiComponent);
void AddChild(UIComponent* uiComponent, UIComponent* child);
UIComponent* FindUIComponent(UIComponent* uiComponent, LPCTSTR name);
void ApplyUIComponent(UIComponent* uiComponent, HWND window);
void EvaluateUIComponent(UIComponent* uiComponent, HWND window, float x, float y, float width, float height);
void UpdateUIComponent(UIComponent* uiComponent);

int GetCenterX(const UIComponent* uiComponent);
int GetX(const UIComponent* uiComponent);
int GetY(const UIComponent* uiComponent);
HFONT GetFont(const UIComponent* uiComponent);

#define UICOMP_BASE(tag, name, window, type, alignment, lengthType, constant, font, parent)		\
UIComponent* tag = calloc(1, sizeof(UIComponent));												\
CreateUIComponent(tag, name);																	\
tag->Type = type;																				\
tag->Alignment = alignment;																		\
tag->Length = calloc(1, sizeof(UILength));														\
tag->Length->Type = lengthType;																	\
tag->Length->Constant = constant;																\
tag->Font = font;																				\
tag->Window = window;																			\
AddChild(parent, tag)
#define UICOMP_WIN(tag, alignment, parent)														\
UIComponent* tag = calloc(1, sizeof(UIComponent));												\
CreateUIComponent(tag, NULL);																	\
tag->Type = Window;																				\
tag->Alignment = alignment;																		\
AddChild(parent, tag)
#define UICOMP_CON(tag, type, alignment, constant, parent)					UICOMP_BASE(tag, NULL, NULL, type, alignment, Constant, constant, NULL, parent)
#define UICOMP_CON_N(tag, name, type, alignment, constant, font, parent)	UICOMP_BASE(tag, name, NULL, type, alignment, Constant, constant, font, parent)
#define UICOMP_CON_W(tag, window, type, alignment, constant, font, parent)	UICOMP_BASE(tag, NULL, window, type, alignment, Constant, constant, font, parent)
#define UICOMP_FCN(tag, type, alignment, constant, parent)					UICOMP_BASE(tag, NULL, NULL, type, alignment, FitConstant, constant, NULL, parent)
#define UICOMP_FCN_N(tag, name, type, alignment, constant, font, parent)	UICOMP_BASE(tag, name, NULL, type, alignment, FitConstant, constant, font, parent)
#define UICOMP_FCN_W(tag, window, type, alignment, constant, font, parent)	UICOMP_BASE(tag, NULL, window, type, alignment, FitConstant, constant, font, parent)
#define UICOMP_DOW(tag, type, alignment, constant, parent)					UICOMP_BASE(tag, NULL, NULL, type, alignment, DependentOnWidth, constant, NULL, parent)
#define UICOMP_DOW_N(tag, name, type, alignment, constant, font, parent)	UICOMP_BASE(tag, name, NULL, type, alignment, DependentOnWidth, constant, font, parent)
#define UICOMP_DOW_W(tag, window, type, alignment, constant, font, parent)	UICOMP_BASE(tag, NULL, window, type, alignment, DependentOnWidth, constant, font, parent)
#define UICOMP_DOH(tag, type, alignment, constant, parent)					UICOMP_BASE(tag, NULL, NULL, type, alignment, DependentOnHeight, constant, NULL, parent)
#define UICOMP_DOH_N(tag, name, type, alignment, constant, font, parent)	UICOMP_BASE(tag, name, NULL, type, alignment, DependentOnHeight, constant, font, parent)
#define UICOMP_DOH_W(tag, window, type, alignment, constant, font, parent)	UICOMP_BASE(tag, NULL, window, type, alignment, DependentOnHeight, constant, font, parent)
#define UICOMP_DOC(tag, type, alignment, parent)							UICOMP_BASE(tag, NULL, NULL, type, alignment, DependentOnChildren, 0, NULL, parent)
#define UICOMP_DOC_N(tag, name, type, alignment, font, parent)				UICOMP_BASE(tag, name, NULL, type, alignment, DependentOnChildren, 0, font, parent)
#define UICOMP_DOC_W(tag, window, type, alignment, font, parent)			UICOMP_BASE(tag, NULL, window, type, alignment, DependentOnChildren, 0, font, parent)

#define UIMARG_MEMB(location, margin) location##margin
#define UIMARG_BASE(tag, location, type, constant)												\
tag-> UIMARG_MEMB(location, Margin) = calloc(1, sizeof(UILength));								\
tag-> UIMARG_MEMB(location, Margin) ->Type = type;												\
tag-> UIMARG_MEMB(location, Margin) ->Constant = constant
#define UIMARG_CON(tag, location, constant) UIMARG_BASE(tag, location, Constant, constant)
#define UIMARG_CON_LR(tag, constant) UIMARG_CON(tag, Left, constant); UIMARG_CON(tag, Right, constant)
#define UIMARG_CON_TB(tag, constant) UIMARG_CON(tag, Top, constant); UIMARG_CON(tag, Bottom, constant)
#define UIMARG_CON_AS(tag, constant) UIMARG_CON_LR(tag, constant); UIMARG_CON_TB(tag, constant)
#define UIMARG_DOW(tag, location, constant) UIMARG_BASE(tag, location, DependentOnWidth, constant)

#define UICOMP_FIND(tag, name) UIComponent* const tag = FindUIComponent(&uiEngine->RootComponent, name)

typedef struct {
	Array Fonts;
	UIComponent RootComponent;
} UIEngine;

void CreateUIEngine(UIEngine* uiEngine);
void DestroyUIEngine(UIEngine* uiEngine);
void AddFont(UIEngine* uiEngine, UIComponentFont* uiComponentFont);
void EvaluateUIEngine(UIEngine* uiEngine, HWND window, int width, int height);
void UpdateUIEngine(UIEngine* uiEngine);