#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include <Windows.h>

#include "Window.h"

int APIENTRY WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR cmdArgs, int cmdShow) {
	Initialize(instance);
	(void)(prevInstance, cmdArgs);
	MainWindow = CreateAndShowWindow(_T("MainWindow"), _T("단어 암기 프로그램"), cmdShow);

	MSG message;
	while (GetMessage(&message, NULL, 0, 0)) {
		if (IsDialogMessage(VocabularyWindow, &message) == 0) {
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
	}
	return (int)message.wParam;
}