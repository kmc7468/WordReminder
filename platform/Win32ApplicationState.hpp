#pragma once

#include "Win32API.hpp"

#include "../pal/ApplicationState.hpp"

#include <string>

class Win32ApplicationState final : public ApplicationState {
private:
	HINSTANCE m_Instance;

public:
	Win32ApplicationState(HINSTANCE instance) noexcept;
	Win32ApplicationState(const Win32ApplicationState&) = delete;
	virtual ~Win32ApplicationState() override = default;

public:
	Win32ApplicationState& operator=(const Win32ApplicationState&) = delete;

public:
	virtual bool Initialize() override;
	virtual int Run() override;
	virtual void Finalize() override;

	HINSTANCE GetInstance() const noexcept;

private:
	bool CreateWindowClass(const std::wstring& className, WNDPROC wndProc) noexcept;
};