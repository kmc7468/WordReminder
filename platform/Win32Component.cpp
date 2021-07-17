#include "Win32Component.hpp"

#include <cassert>

Win32Component::Win32Component(std::wstring className, DWORD style) noexcept
	: m_ClassName(std::move(className)), m_Style(style) {
	assert(!m_ClassName.empty());
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

void Win32Component::CreateComponent(Point location, Size size, const std::wstring& text, bool visibility, std::size_t index) {
	Win32Component* const parent = dynamic_cast<Win32Component*>(GetParent());
	m_Window = CreateWindowExW(0, m_ClassName.data(), text.data(), m_Style | (visibility ? WS_VISIBLE : 0),
		location.X, location.Y, size.Width, size.Height,
		parent ? parent->m_Window : nullptr, reinterpret_cast<HMENU>(index), /*TODO: hInstance*/nullptr, nullptr);
	SetWindowLongPtrW(m_Window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
}

HWND Win32Component::GetHandle() noexcept {
	return m_Window;
}

std::pair<Point, Size> Win32Component::GetRectangle() const noexcept {
	RECT rectangle;
	GetWindowRect(m_Window, &rectangle);
	return std::make_pair<Point, Size>({ rectangle.left, rectangle.top }, { rectangle.right - rectangle.left, rectangle.bottom - rectangle.top });
}

Win32Window::Win32Window()
	: Win32Component(L"Window", WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN) {}

LRESULT Win32Window::WndProc(UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
		// TODO
	}
	return DefWindowProcW(GetHandle(), message, wParam, lParam);
}
LRESULT CALLBACK Win32Window::WndProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam) {
	if (const auto window = dynamic_cast<Win32Window*>(reinterpret_cast<Win32Component*>(GetWindowLongPtrW(handle, GWLP_USERDATA)));
		window) return window->WndProc(message, wParam, lParam);
	else return DefWindowProcW(handle, message, wParam, lParam);
}