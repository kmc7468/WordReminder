#pragma once

#include "Win32API.hpp"

#include "../pal/Component.hpp"

#include <string>
#include <utility>

class Win32Window;

class Win32Component : public virtual Component {
	friend class Win32Window;

private:
	HWND m_Window = nullptr;

	std::wstring m_ClassName;
	DWORD m_Style = 0, m_ExStyle = 0;
	HMENU m_Menu = nullptr;

public:
	Win32Component(std::wstring className, DWORD style, DWORD exStyle) noexcept;
	Win32Component(const Win32Component&) = delete;
	virtual ~Win32Component() override = 0;

public:
	Win32Component& operator=(const Win32Component&) = delete;

protected:
	virtual void SetParentDirect(Component* parent, std::size_t index) override;

	virtual Point GetLocationDirect() const override;
	virtual void SetLocationDirect(Point newLocation) override;
	virtual Size GetSizeDirect() const override;
	virtual void SetSizeDirect(Size newSize) override;
	virtual std::wstring GetTextDirect() const override;
	virtual void SetTextDirect(const std::wstring& newText) override;
	virtual bool GetVisibilityDirect() const override;
	virtual void SetVisibilityDirect(bool newVisibility) override;

	virtual void CreateComponent(Point location, Size size, const std::wstring& text, bool visibility) override;

public:
	HWND GetHandle() noexcept;

private:
	std::pair<Point, Size> GetRectangle() const noexcept;
};

class Win32ApplicationState;

class Win32Window final : public Window, public Win32Component {
	friend class Win32ApplicationState;

public:
	Win32Window(std::unique_ptr<EventHandler>&& eventHandler, int width, int height);
	Win32Window(const Win32Window&) = delete;
	virtual ~Win32Window() override = default;

public:
	Win32Window& operator=(const Win32Window&) = delete;

private:
	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK WndProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);
};