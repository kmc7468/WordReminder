#pragma once

#include "Win32API.hpp"

#include "../pal/Font.hpp"

class Win32Font final : public Font {
private:
	HFONT m_Font = nullptr;

public:
	Win32Font(std::wstring name, int height, bool isBold) noexcept;
	Win32Font(const Win32Font&) = delete;
	virtual ~Win32Font() override;

public:
	Win32Font& operator=(const Win32Font&) = delete;

public:
	virtual bool IsCreated() const noexcept override;

	HFONT GetHandle() const noexcept;
};