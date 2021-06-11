#pragma comment(linker, "\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include "Application.h"
#include "WinAPI.h"
#include "Window.h"

#include <stdlib.h>

int APIENTRY wWinMain(HINSTANCE instance, HINSTANCE prevInstance, LPWSTR cmdArgs, int cmdShow) {
	if (!InitializeApplication(instance)) {
		(void)(prevInstance, cmdArgs);
		MessageBox(NULL, _T("���α׷��� �ʱ�ȭ�ϴ� �� ������ �߻��߽��ϴ�."), _T("����"), MB_OK | MB_ICONERROR);
		return EXIT_FAILURE;
	} else if (ProcessCommandLineArguments(cmdArgs)) return EXIT_SUCCESS;
	ShowWindow(MainWindow, cmdShow);

	MSG message;
	while (GetMessage(&message, NULL, 0, 0)) {
		if (IsDialogMessage((HWND)GetScene(MainWindow), &message) == 0 &&
			IsDialogMessage((HWND)GetScene(DialogWindow), &message) == 0) {
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
	}

	DestroyApplication();
	return (int)message.wParam;
}