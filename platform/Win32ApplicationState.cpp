#include "Win32ApplicationState.hpp"

#include "Win32Component.hpp"

#include <cassert>

Win32ApplicationState::Win32ApplicationState(HINSTANCE instance) noexcept
	: m_Instance(instance) {
	assert(instance != nullptr);
}

bool Win32ApplicationState::Initialize() {
	return CreateWindowClass(L"Window", Win32Window::WndProc);
}
int Win32ApplicationState::Run() {
	MSG message;
	while (GetMessageW(&message, nullptr, 0, 0)) {
		TranslateMessage(&message);
		DispatchMessageW(&message);
	}
	return static_cast<int>(message.wParam);
}
void Win32ApplicationState::Finalize() {}

HINSTANCE Win32ApplicationState::GetInstance() const noexcept {
	return m_Instance;
}

bool Win32ApplicationState::CreateWindowClass(const std::wstring& className, WNDPROC wndProc) noexcept {
	WNDCLASSW wc = { 0 };
	wc.hbrBackground = static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hInstance = m_Instance;
	wc.lpfnWndProc = wndProc;
	wc.lpszClassName = className.data();
	wc.lpszMenuName = nullptr;
	wc.style = CS_VREDRAW | CS_HREDRAW;

	return RegisterClassW(&wc);
}