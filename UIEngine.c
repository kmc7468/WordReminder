#include "UIEngine.h"

#include "Window.h"

#include <math.h>
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

	case Sum:
		uiLength->Evaluated = 0;
		for (int i = 0; i < uiLength->Terms.Count; ++i) {
			UILength* const term = *(UILength**)GetElement(&uiLength->Terms, i);

			EvaluateUILength(term, window, width, height);
			uiLength->Evaluated += term->Evaluated;
		}
		break;
	}
}

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

	float childrenLengthSum = 0;
	for (int i = 0; i < uiComponent->Children.Count; ++i) {
		UIComponent* const child = *(UIComponent**)GetElement(&uiComponent->Children, i);

		if (child->Type != Window) {
			EvaluateUILength(child->Length, window, width, height);
		}

		switch (child->Type) {
		case Window:
			EvaluateUIComponent(child, window, x, y, width, height);
			break;

		case Horizontal:
			EvaluateUIComponent(child, window, x, y + childrenLengthSum, width, child->Length->Evaluated);
			break;

		case Vertical:
			EvaluateUIComponent(child, window, x + childrenLengthSum, y, child->Length->Evaluated, height);
			break;
		}

		if (child->Type != Window && child->Length->Type == DependentOnChildren) {
			child->Length->Evaluated = 0;
			for (int j = 0; j < child->Children.Count; ++j) {
				child->Length->Evaluated += (*(UIComponent**)GetElement(&child->Children, j))->Length->Evaluated;
			}
		}
		childrenLengthSum += child->Length->Evaluated;
	}

	switch (uiComponent->Alignment) {
	case Center:
		if ((*(UIComponent**)GetElement(&uiComponent->Children, 0))->Type == Horizontal) {
			MoveChildrenUIComponent(uiComponent, 0, (height - childrenLengthSum) / 2.f);
		} else {
			MoveChildrenUIComponent(uiComponent, (width - childrenLengthSum) / 2.f, 0);
		}
		break;

	case Bottom:
		if ((*(UIComponent**)GetElement(&uiComponent->Children, 0))->Type == Horizontal) {
			MoveChildrenUIComponent(uiComponent, 0, height - childrenLengthSum);
		} else {
			MoveChildrenUIComponent(uiComponent, width - childrenLengthSum, 0);
		}
		break;
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