#pragma comment(linker, "\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include "Application.h"
#include "WinAPI.h"
#include "Window.h"

#include <stdlib.h>

int APIENTRY WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR cmdArgs, int cmdShow) {
	if (!InitializeApplication(instance)) {
		(void)(prevInstance, cmdArgs);
		MessageBox(NULL, _T("프로그램을 초기화하는 중 오류가 발생했습니다."), _T("오류"), MB_OK | MB_ICONERROR);
		return EXIT_FAILURE;
	}
	ShowWindow(MainWindow, cmdShow);

	MSG message;
	while (GetMessage(&message, NULL, 0, 0)) {
		TranslateMessage(&message);
		DispatchMessage(&message);
	}
	return (int)message.wParam;
}