#pragma once

#include "Win32API.hpp"

#include "../pal/Component.hpp"

#include <string>
#include <utility>

class Win32Component : public virtual Component {
private:
	HWND m_Window = nullptr;

	std::wstring m_ClassName;
	DWORD m_Style = 0;

protected:
	Win32Component(std::wstring className, DWORD style) noexcept;

protected:
	virtual Point GetLocationDirect() const override;
	virtual void SetLocationDirect(Point newLocation) override;
	virtual Size GetSizeDirect() const override;
	virtual void SetSizeDirect(Size newSize) override;
	virtual std::wstring GetTextDirect() const override;
	virtual void SetTextDirect(const std::wstring& newText) override;
	virtual bool GetVisibilityDirect() const override;
	virtual void SetVisibilityDirect(bool newVisibility) override;

	virtual void CreateComponent(Point location, Size size, const std::wstring& text, bool visibility, std::size_t index) override;

public:
	HWND GetHandle() noexcept;

private:
	std::pair<Point, Size> GetRectangle() const noexcept;
};

class Win32ApplicationState;

class Win32Window : public Window, public Win32Component {
	friend class Win32ApplicationState;

public:
	Win32Window();
	Win32Window(const Win32Window&) = delete;
	virtual ~Win32Window() override = default;

public:
	Win32Window& operator=(const Win32Window&) = delete;

private:
	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK WndProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);
};