#include "Config.hpp"
#include "Utility.hpp"

#ifdef _WIN32
#	include "../platform/Win32.hpp"

std::unique_ptr<Config> Config::Create() {
	return std::make_unique<Win32RegistryKey>();
}

std::u8string EncodeToUTF8(const std::wstring_view& string) {
	const int length = WideCharToMultiByte(CP_UTF8, 0, string.data(), static_cast<int>(string.size()) + 1, nullptr, 0, nullptr, nullptr);
	std::u8string result(length - 1, 0);

	WideCharToMultiByte(CP_UTF8, 0, string.data(), static_cast<int>(string.size()) + 1, reinterpret_cast<LPSTR>(result.data()), length, nullptr, nullptr);
	return result;
}
#else
#	error 지원하지 않는 플랫폼입니다.
#endif