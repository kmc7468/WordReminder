#pragma once

#include "UIEngine.h"
#include "WinAPI.h"

#include <stdbool.h>

void RegisterWindow(LPCTSTR className, WNDPROC wndProc);
HWND CreateChild(LPCTSTR className, LPCTSTR text, HFONT font, int flags, int x, int y, int w, int h, HWND parent, int menu);
HWND CreateButton(LPCTSTR text, int flags, HWND parent, int menu);
HWND CreateCheckBox(LPCTSTR text, int flags, HWND parent, int menu);
HWND CreateRadioButton(LPCTSTR text, int flags, HWND parent, int menu);
HWND CreateStatic(LPCTSTR text, int flags, HWND parent, int menu);
HWND CreateList(int flags, HWND parent, int menu);
HWND CreateEdit(int flags, HWND parent, int menu);
void SetFont(HWND window, HFONT font);

extern HWND MainWindow, DialogWindow;

HWND CreateSceneWindow(SUBCLASSPROC windowProc, SUBCLASSPROC sceneProc);

HWND CreateScene(HWND window, SUBCLASSPROC sceneProc);
HWND GetScene(HWND window);
HWND ChangeScene(HWND window, HWND newScene);
void SetSceneTitle(HWND scene, LPCTSTR newTitle);
UIEngine* GetUIEngine(HWND scene);

float GetAppropriateFloatLengthForDpi(HWND window, float originalLength);
int GetAppropriateLengthForDpi(HWND window, int originalLength);
float GetAppropriateFloatLengthForSize(HWND window, float originalLength);
int GetAppropriateLengthForSize(HWND window, int originalLength);

typedef struct {
	HDC OriginalDC;
	HBITMAP OriginalBitmap;
	PAINTSTRUCT OriginalPaintStruct;

	HDC BufferDC;
	int Width, Height;
} PaintContext;

HDC StartPaint(HWND window, int width, int height, PaintContext* paintContext);
void StopPaint(HWND window, PaintContext* paintContext);

void DrawString(HDC dc, HFONT font, int x, int y, LPCTSTR string, int length);

#define START_PAINT																				\
UIEngine* const uiEngine = (UIEngine*)lParam;													\
PaintContext paintContext;																		\
const HDC dc = StartPaint(handle, WIDTH, HEIGHT, &paintContext)
#define STOP_PAINT																				\
StopPaint(handle, &paintContext);																\
return 0

#define EVENT																					\
(void)(dummy0, dummy1);																			\
RECT clientRect;																				\
GetClientRect(handle, &clientRect);																\
switch (message)
#define WIDTH clientRect.right
#define HEIGHT clientRect.bottom
#define CSTR(string) _T(string), ARRAYSIZE(_T(string)) - 1

#define AM_CREATE			WM_APP + 0
#define AM_CREATEUI			WM_APP + 1
#define AM_CHANGESCENE		WM_APP + 2
#define AM_ACTIVATE			WM_APP + 3
#define AM_DEACTIVATE		WM_APP + 4
#define AM_DESTROY			WM_APP + 5
#define AM_PAINT			WM_APP + 6
#define AM_DATA				WM_APP + 7

#define DT_QUESTIONOPTION	(WPARAM)0
#define DT_VOCABULARY		(WPARAM)1

#define AM_USER				WM_USER + 2

LRESULT CALLBACK SceneWindowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK MainWindowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR dummy0, DWORD_PTR dummy1);

LRESULT CALLBACK SceneProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK MainSceneProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR dummy0, DWORD_PTR dummy1);
LRESULT CALLBACK QuestionOptionSceneProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR dummy0, DWORD_PTR dummy1);
LRESULT CALLBACK QuestionSceneProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR dummy0, DWORD_PTR dummy1);
LRESULT CALLBACK VocabularySceneProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR dummy0, DWORD_PTR dummy1);
LRESULT CALLBACK StatisticSceneProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR dummy0, DWORD_PTR dummy1);