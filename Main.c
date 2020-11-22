#pragma comment(linker, "\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include <WinSock2.h>
#include <Windows.h>

#include "Window.h"

int APIENTRY WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR cmdArgs, int cmdShow) {
	if (!Initialize(instance)) {
		(void)(prevInstance, cmdArgs);
		MessageBox(NULL, _T("���α׷��� �ʱ�ȭ�ϴ� �� ������ �߻��߽��ϴ�."), _T("����"), MB_OK | MB_ICONERROR);
		return 0;
	}
	MainWindow = CreateAndShowWindow(_T("MainWindow"), _T("�ܾ� �ϱ� ���α׷�"), cmdShow);

	MSG message;
	while (GetMessage(&message, NULL, 0, 0)) {
		if (IsDialogMessage(VocabularyWindow, &message) == 0 &&
			IsDialogMessage(OnlineMultiplayWindow, &message) == 0) {
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
	}
	return (int)message.wParam;
}