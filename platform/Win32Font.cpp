#include "Win32Font.hpp"

#include <cassert>
#include <string_view>

Win32Font::Win32Font(std::wstring name, int height, bool isBold) noexcept
	: Win32Font(std::move(name), height, isBold) {
	const std::wstring_view nameView = GetName();
	assert(nameView.size() < 31);

	m_Font = CreateFontW(height, 0, 0, 0, isBold ? FW_BOLD : FW_NORMAL,
		false, false, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		FF_DONTCARE | DEFAULT_PITCH, name.data());
}
Win32Font::~Win32Font() {
	DeleteObject(m_Font);
}

bool Win32Font::IsCreated() const noexcept {
	return m_Font != nullptr;
}

HFONT Win32Font::GetHandle() const noexcept {
	return m_Font;
}