#include "Win32Component.hpp"

#include "Win32ApplicationState.hpp"

#include "../ui/Application.hpp"

#include <cassert>

Win32Component::Win32Component(std::wstring className, DWORD style, DWORD exStyle) noexcept
	: m_ClassName(std::move(className)), m_Style(style), m_ExStyle(exStyle) {
	assert(!m_ClassName.empty());
}
Win32Component::~Win32Component() {}

void Win32Component::SetParentDirect(Component*, std::size_t index) {
	m_Menu = reinterpret_cast<HMENU>(index);
}

Point Win32Component::GetLocationDirect() const {
	return GetRectangle().first;
}
void Win32Component::SetLocationDirect(Point newLocation) {
	SetWindowPos(m_Window, nullptr, newLocation.X, newLocation.Y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
}
Size Win32Component::GetSizeDirect() const {
	return GetRectangle().second;
}
void Win32Component::SetSizeDirect(Size newSize) {
	SetWindowPos(m_Window, nullptr, 0, 0, newSize.Width, newSize.Height, SWP_NOZORDER | SWP_NOMOVE);
}
std::wstring Win32Component::GetTextDirect() const {
	const int length = GetWindowTextLengthW(m_Window);
	if (length == 0) return {};

	std::wstring result(length, 0);
	GetWindowTextW(m_Window, result.data(), length + 1);
	return result;
}
void Win32Component::SetTextDirect(const std::wstring& newText) {
	SetWindowTextW(m_Window, newText.data());
}
bool Win32Component::GetVisibilityDirect() const {
	return IsWindowVisible(m_Window);
}
void Win32Component::SetVisibilityDirect(bool newVisibility) {
	ShowWindow(m_Window, newVisibility ? SW_SHOW : SW_HIDE);
}

void Win32Component::CreateComponent(Point location, Size size, const std::wstring& text, bool visibility) {
	Win32Component* const parent = dynamic_cast<Win32Component*>(GetParent());
	m_Window = CreateWindowExW(m_ExStyle, m_ClassName.data(), text.data(), m_Style | (visibility ? WS_VISIBLE : 0),
		location.X, location.Y, size.Width, size.Height,
		parent ? parent->m_Window : nullptr, m_Menu,
		Application::Get().GetApplicationState<Win32ApplicationState>()->GetInstance(), this);
}

HWND Win32Component::GetHandle() noexcept {
	return m_Window;
}

std::pair<Point, Size> Win32Component::GetRectangle() const noexcept {
	RECT rectangle;
	GetWindowRect(m_Window, &rectangle);
	return std::make_pair<Point, Size>({ rectangle.left, rectangle.top }, { rectangle.right - rectangle.left, rectangle.bottom - rectangle.top });
}

Win32Window::Win32Window(std::unique_ptr<EventHandler>&& eventHandler, int width, int height)
	: Component(std::move(eventHandler)), Window({ CW_USEDEFAULT, CW_USEDEFAULT }, { width, height }),
	Win32Component(L"Window", WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0) {}

LRESULT Win32Window::WndProc(UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_CREATE:
		OnCreate();
		return 0;

	case WM_CLOSE:
		OnClose();
		return 0;
	}
	return DefWindowProcW(GetHandle(), message, wParam, lParam);
}
LRESULT CALLBACK Win32Window::WndProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam) {
	Win32Window* window = reinterpret_cast<Win32Window*>(GetWindowLongPtrW(handle, GWLP_USERDATA));
	if (message == WM_CREATE) {
		CREATESTRUCTW* const createStruct = reinterpret_cast<CREATESTRUCTW*>(lParam);
		window = dynamic_cast<Win32Window*>(static_cast<Win32Component*>(createStruct->lpCreateParams));
		window->m_Window = handle;
		SetWindowLongPtrW(handle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
	}

	return window ? window->WndProc(message, wParam, lParam) : DefWindowProcW(handle, message, wParam, lParam);
}