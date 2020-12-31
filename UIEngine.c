#include "UIEngine.h"

#include "Window.h"

#include <math.h>
#include <stdbool.h>
#include <string.h>

void CreateUILength(UILength* uiLength) {
	CreateArray(&uiLength->Terms, sizeof(UILength*));
}
void DestroyUILength(UILength* uiLength) {
	for (int i = 0; i < uiLength->Terms.Count; ++i) {
		UILength* const length = *(UILength**)GetElement(&uiLength->Terms, i);

		DestroyUILength(length);
		free(length);
	}
	DestroyArray(&uiLength->Terms);
}
void AddTerm(UILength* uiLength, UILength* term) {
	AddElement(&uiLength->Terms, &term);
}
void EvaluateUILength(UILength* uiLength, HWND window, float width, float height) {
	switch (uiLength->Type) {
	case Constant:
		uiLength->Evaluated = (float)GetAppropriateLengthForDpi(window, uiLength->Constant);
		break;

	case DependentOnWidth:
		uiLength->Evaluated = width * uiLength->Constant / 100.f;
		break;

	case DependentOnHeight:
		uiLength->Evaluated = height * uiLength->Constant / 100.f;
		break;
	}
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

	if (uiComponent->Length) {
		DestroyUILength(uiComponent->Length);
		free(uiComponent->Length);
	}

	for (int i = 0; i < uiComponent->Children.Count; ++i) {
		UIComponent* const component = *(UIComponent**)GetElement(&uiComponent->Children, i);

		DestroyUIComponent(component);
		free(component);
	}
	DestroyArray(&uiComponent->Children);
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
}
void EvaluateUIComponent(UIComponent* uiComponent, HWND window, float x, float y, float width, float height) {
	uiComponent->EvaluatedX = x;
	uiComponent->EvaluatedY = y;
	uiComponent->EvaluatedWidth = width;
	uiComponent->EvaluatedHeight = height;

	bool hasHorizontal = false, hasVertical = false;
	for (int i = 0; i < uiComponent->Children.Count; ++i) {
		UIComponent* const child = *(UIComponent**)GetElement(&uiComponent->Children, i);
		switch (child->Type) {
		case Window:
			EvaluateUIComponent(child, window, x, y, width, height);

			hasHorizontal = true;
			hasVertical = true;
			break;

		case Horizontal:
			EvaluateUILength(child->Length, window, uiComponent->EvaluatedWidth, uiComponent->EvaluatedHeight);
			EvaluateUIComponent(child, window, x, y, width, child->Length->Evaluated);

			y += child->EvaluatedHeight;
			height -= child->EvaluatedHeight;
			hasHorizontal = true;
			break;

		case Vertical:
			EvaluateUILength(child->Length, window, uiComponent->EvaluatedWidth, uiComponent->EvaluatedHeight);
			EvaluateUIComponent(child, window, x, y, child->Length->Evaluated, height);

			x += child->EvaluatedWidth;
			width -= child->EvaluatedWidth;
			hasVertical = true;
			break;
		}
	}

	if (hasHorizontal) {
		x = uiComponent->EvaluatedX + uiComponent->EvaluatedWidth;
		width = 0;
	}
	if (hasVertical) {
		y = uiComponent->EvaluatedY + uiComponent->EvaluatedHeight;
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
		uiComponent->EvaluatedWidth = fabsf(width);
		uiComponent->EvaluatedHeight = fabsf(height);
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

void CreateUIEngine(UIComponent* uiEngine) {
	CreateUIComponent(uiEngine, NULL);
}
void EvaluateUIEngine(UIComponent* uiEngine, HWND window, int width, int height) {
	EvaluateUIComponent(uiEngine, window, 0, 0, (float)width, (float)height);
}
void UpdateUIEngine(UIComponent* uiEngine) {
	UpdateUIComponent(uiEngine);
}
void DestroyUIEngine(UIComponent* uiEngine) {
	DestroyUIComponent(uiEngine);
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