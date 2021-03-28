#include "UIEngine.h"

#include "Application.h"
#include "Window.h"

#include <math.h>
#include <string.h>

void EvaluateUILength(UILength* uiLength, HWND window, float width, float height) {
	switch (uiLength->Type) {
	case Constant:
		uiLength->Evaluated = GetAppropriateFloatLengthForDpi(window, uiLength->Constant);
		break;

	case FitConstant:
		uiLength->Evaluated = GetAppropriateFloatLengthForSize(window, uiLength->Constant);
		break;

	case DependentOnWidth:
		uiLength->Evaluated = width * uiLength->Constant / 100.f;
		break;

	case DependentOnHeight:
		uiLength->Evaluated = height * uiLength->Constant / 100.f;
		break;
	}
}

void EvaluateUIComponentFont(UIComponentFont* uiComponentFont, HWND window, int width, int height) {
	EvaluateUILength(uiComponentFont->Height, window, (float)width, (float)height);
	const int evaluated = (int)floorf(uiComponentFont->Height->Evaluated + 0.5f);
	if (evaluated != uiComponentFont->EvaluatedHeight) {
		uiComponentFont->EvaluatedHeight = evaluated;
		uiComponentFont->ShouldCreate = true;
	} else {
		uiComponentFont->ShouldCreate = false;
	}
}
void UpdateUIComponentFont(UIComponentFont* uiComponentFont) {
	if (uiComponentFont->ShouldCreate) {
		DeleteObject(uiComponentFont->Font);
		uiComponentFont->Font = CreateGlobalFont(uiComponentFont->EvaluatedHeight, uiComponentFont->IsBold);
	}
}
void DestroyUIComponentFont(UIComponentFont* uiComponentFont) {
	free(uiComponentFont->Height);
	DeleteObject(uiComponentFont->Font);
}

static void MoveUIComponent(UIComponent* uiComponent, float xDelta, float yDelta);
static void MoveChildrenUIComponent(UIComponent* uiComponent, float xDelta, float yDelta);

void CreateUIComponent(UIComponent* uiComponent, LPCTSTR name) {
	if (name) {
		uiComponent->Name = malloc(sizeof(TCHAR) * (_tcslen(name) + 1));
		_tcscpy(uiComponent->Name, name);
	}

	CreateArray(&uiComponent->Children, sizeof(UIComponent*));
}
void DestroyUIComponent(UIComponent* uiComponent) {
	free(uiComponent->Name);

	free(uiComponent->Length);

	for (int i = 0; i < uiComponent->Children.Count; ++i) {
		UIComponent* const component = *(UIComponent**)GetElement(&uiComponent->Children, i);

		DestroyUIComponent(component);
		free(component);
	}
	DestroyArray(&uiComponent->Children);

	free(uiComponent->LeftMargin);
	free(uiComponent->RightMargin);
	free(uiComponent->TopMargin);
	free(uiComponent->BottomMargin);
}
void AddChild(UIComponent* uiComponent, UIComponent* child) {
	AddElement(&uiComponent->Children, &child);
}
UIComponent* FindUIComponent(UIComponent* uiComponent, LPCTSTR name) {
	if (uiComponent->Name && _tcscmp(uiComponent->Name, name) == 0) return uiComponent;

	for (int i = 0; i < uiComponent->Children.Count; ++i) {
		UIComponent* const child = *(UIComponent**)GetElement(&uiComponent->Children, i);

		UIComponent* const result = FindUIComponent(child, name);
		if (result) return result;
	}
	return NULL;
}
void ApplyUIComponent(UIComponent* uiComponent, HWND window) {
	SetWindowPos(window, NULL, (int)floorf(uiComponent->EvaluatedX + 0.5f), (int)floorf(uiComponent->EvaluatedY + 0.5f),
		(int)floorf(uiComponent->EvaluatedWidth + 0.5f), (int)floorf(uiComponent->EvaluatedHeight + 0.5f), SWP_NOZORDER);
	SetFont(window, uiComponent->Font->Font);
}
void EvaluateUIComponent(UIComponent* uiComponent, HWND window, float x, float y, float width, float height) {
	float xDelta = 0, yDelta = 0, widthDelta = 0, heightDelta = 0;
	if (uiComponent->LeftMargin) {
		EvaluateUILength(uiComponent->LeftMargin, window, width, height);
		xDelta = uiComponent->LeftMargin->Evaluated;
		widthDelta += uiComponent->LeftMargin->Evaluated;
	}
	if (uiComponent->RightMargin) {
		EvaluateUILength(uiComponent->RightMargin, window, width, height);
		widthDelta += fabsf(uiComponent->RightMargin->Evaluated);
	}
	if (uiComponent->TopMargin) {
		EvaluateUILength(uiComponent->TopMargin, window, width, height);
		yDelta = uiComponent->TopMargin->Evaluated;
		heightDelta += uiComponent->TopMargin->Evaluated;
	}
	if (uiComponent->BottomMargin) {
		EvaluateUILength(uiComponent->BottomMargin, window, width, height);
		heightDelta += fabsf(uiComponent->BottomMargin->Evaluated);
	}

	uiComponent->EvaluatedX = (x += xDelta);
	uiComponent->EvaluatedY = (y += yDelta);
	uiComponent->EvaluatedWidth = (width -= widthDelta);
	uiComponent->EvaluatedHeight = (height -= heightDelta);
	uiComponent->EvaluatedVirtualWidth = uiComponent->EvaluatedWidth + (uiComponent->RightMargin && uiComponent->RightMargin->Evaluated > 0 ? widthDelta : xDelta);
	uiComponent->EvaluatedVirtualHeight = uiComponent->EvaluatedHeight + (uiComponent->BottomMargin && uiComponent->BottomMargin->Evaluated > 0 ? heightDelta : yDelta);

	UIComponent* prevChild = NULL;
	bool hasHorizontal = false, hasVertical = false;
	for (int i = 0; i < uiComponent->Children.Count; ++i) {
		UIComponent* const child = *(UIComponent**)GetElement(&uiComponent->Children, i);
		switch (child->Type) {
		case Window:
			EvaluateUIComponent(child, window, x, y, fabsf(width), fabsf(height));
			break;

		case Horizontal:
			EvaluateUILength(child->Length, window, uiComponent->EvaluatedWidth, uiComponent->EvaluatedHeight);
			EvaluateUIComponent(child, window, x, y, width, child->Length->Evaluated);
			break;

		case Vertical:
			EvaluateUILength(child->Length, window, uiComponent->EvaluatedWidth, uiComponent->EvaluatedHeight);
			EvaluateUIComponent(child, window, x, y, child->Length->Evaluated, height);
			break;
		}

		/*bool isHorizontal = false, isVertical = true;
		if (child->Type == Window) {
			if (fabsf(height - child->EvaluatedVirtualHeight) < 0.0001f) {
				isHorizontal = true;
			}
			if (fabsf(width - child->EvaluatedVirtualWidth) < 0.0001f) {
				isVertical = true;
			}
		}*/

		if (child->Type == Horizontal || child->Type == Window) {
			if (!child->IsOverridable) {
				y += child->EvaluatedVirtualHeight;
			}

			if (!prevChild || !prevChild->IsOverridable) {
				height -= child->EvaluatedVirtualHeight;
			} else {
				height -= max(0, child->EvaluatedVirtualHeight - prevChild->EvaluatedVirtualHeight);
			}
			hasHorizontal = true;
		}
		if (child->Type == Vertical) {
			if (!child->IsOverridable) {
				x += child->EvaluatedVirtualWidth;
			}

			if (!prevChild || !prevChild->IsOverridable) {
				width -= child->EvaluatedVirtualWidth;
			} else {
				width -= max(0, child->EvaluatedVirtualWidth - prevChild->EvaluatedVirtualWidth);
			}
			hasVertical = true;
		}

		prevChild = child;
	}

	if (hasHorizontal) {
		width = 0;
	}
	if (hasVertical) {
		height = 0;
	}

	switch (uiComponent->Alignment) {
	case Center:
		MoveChildrenUIComponent(uiComponent, width / 2, height / 2);
		break;

	case CenterWithMargin: {
		const float widthMargin = width / (uiComponent->Children.Count - 1);
		const float heightMargin = height / (uiComponent->Children.Count - 1);
		for (int i = 0; i < uiComponent->Children.Count; ++i) {
			MoveUIComponent(*(UIComponent**)GetElement(&uiComponent->Children, i), widthMargin * i, heightMargin * i);
		}
		break;
	}

	case Bottom:
		MoveChildrenUIComponent(uiComponent, width, height);
		break;
	}

	if (uiComponent->Type != Window && uiComponent->Length->Type == DependentOnChildren) {
		if (uiComponent->Type == Horizontal) {
			uiComponent->EvaluatedVirtualHeight = (uiComponent->EvaluatedHeight = fabsf(height)) + yDelta;
		} else {
			uiComponent->EvaluatedVirtualWidth = (uiComponent->EvaluatedWidth = fabsf(width)) + xDelta;
		}
	}
}
void UpdateUIComponent(UIComponent* uiComponent) {
	if (uiComponent->Window && *uiComponent->Window) {
		ApplyUIComponent(uiComponent, *uiComponent->Window);
	}

	for (int i = 0; i < uiComponent->Children.Count; ++i) {
		UpdateUIComponent(*(UIComponent**)GetElement(&uiComponent->Children, i));
	}
}

void MoveUIComponent(UIComponent* uiComponent, float xDelta, float yDelta) {
	uiComponent->EvaluatedX += xDelta;
	uiComponent->EvaluatedY += yDelta;

	MoveChildrenUIComponent(uiComponent, xDelta, yDelta);
}
void MoveChildrenUIComponent(UIComponent* uiComponent, float xDelta, float yDelta) {
	for (int i = 0; i < uiComponent->Children.Count; ++i) {
		UIComponent* const child = *(UIComponent**)GetElement(&uiComponent->Children, i);

		child->EvaluatedX += xDelta;
		child->EvaluatedY += yDelta;
		MoveChildrenUIComponent(child, xDelta, yDelta);
	}
}

int GetCenterX(const UIComponent* uiComponent) {
	return (int)floorf(uiComponent->EvaluatedX + uiComponent->EvaluatedWidth / 2.f + 0.5f);
}
int GetX(const UIComponent* uiComponent) {
	return (int)floorf(uiComponent->EvaluatedX + 0.5f);
}
int GetY(const UIComponent* uiComponent) {
	return (int)floorf(uiComponent->EvaluatedY + 0.5f);
}
HFONT GetFont(const UIComponent* uiComponent) {
	return uiComponent->Font->Font;
}

void CreateUIEngine(UIEngine* uiEngine) {
	CreateArray(&uiEngine->Fonts, sizeof(UIComponentFont*));
	CreateUIComponent(&uiEngine->RootComponent, NULL);
}
void DestroyUIEngine(UIEngine* uiEngine) {
	for (int i = 0; i < uiEngine->Fonts.Count; ++i) {
		UIComponentFont* const uiComponentFont = *(UIComponentFont**)GetElement(&uiEngine->Fonts, i);

		DestroyUIComponentFont(uiComponentFont);
		free(uiComponentFont);
	}
	DestroyArray(&uiEngine->Fonts);

	DestroyUIComponent(&uiEngine->RootComponent);
}
void AddFont(UIEngine* uiEngine, UIComponentFont* uiComponentFont) {
	AddElement(&uiEngine->Fonts, &uiComponentFont);
}
void EvaluateUIEngine(UIEngine* uiEngine, HWND window, int width, int height) {
	for (int i = 0; i < uiEngine->Fonts.Count; ++i) {
		EvaluateUIComponentFont(*(UIComponentFont**)GetElement(&uiEngine->Fonts, i), window, width, height);
	}

	EvaluateUIComponent(&uiEngine->RootComponent, window, 0, 0, (float)width, (float)height);
}
void UpdateUIEngine(UIEngine* uiEngine) {
	for (int i = 0; i < uiEngine->Fonts.Count; ++i) {
		UpdateUIComponentFont(*(UIComponentFont**)GetElement(&uiEngine->Fonts, i));
	}

	UpdateUIComponent(&uiEngine->RootComponent);
}