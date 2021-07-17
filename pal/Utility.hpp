#pragma once

#include <string>
#include <string_view>

std::u8string EncodeToUTF8(const std::wstring_view& string);