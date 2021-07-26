#pragma once

#include <memory>
#include <string>
#include <string_view>

class Font {
private:
	std::wstring m_Name;
	int m_Height;
	bool m_IsBold;

public:
	Font(std::wstring name, int height, bool isBold) noexcept;
	Font(const Font&) = delete;
	virtual ~Font() = default;

public:
	Font& operator=(const Font&) = delete;

public:
	std::wstring_view GetName() const noexcept;
	int GetHeight() const noexcept;
	bool IsBold() const noexcept;

	virtual bool IsCreated() const noexcept = 0;

public:
	std::unique_ptr<Font> Create(std::wstring name, int height, bool isBold = false);
};