#include "Font.hpp"

#include <cassert>
#include <utility>

Font::Font(std::wstring name, int height, bool isBold) noexcept
	: m_Name(std::move(name)), m_Height(height), m_IsBold(isBold) {
	assert(height > 0);
}

std::wstring_view Font::GetName() const noexcept {
	return m_Name;
}
int Font::GetHeight() const noexcept {
	return m_Height;
}
bool Font::IsBold() const noexcept {
	return m_IsBold;
}