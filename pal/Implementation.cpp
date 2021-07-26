#include "Component.hpp"
#include "Config.hpp"
#include "Font.hpp"
#include "Utility.hpp"

#ifdef _WIN32
#	include "../platform/Win32.hpp"

// Component.hpp
std::unique_ptr<Window> Window::Create(std::unique_ptr<EventHandler>&& eventHandler, int width, int height) {
	return std::make_unique<Win32Window>(std::move(eventHandler), width, height);
}
std::unique_ptr<Button> Button::Create(std::unique_ptr<ButtonEventHandler>&& eventHandler) {
	return std::make_unique<Win32Button>(std::move(eventHandler));
}
std::unique_ptr<Label> Label::Create(std::unique_ptr<EventHandler>&& eventHandler) {
	return std::make_unique<Win32Static>(std::move(eventHandler));
}

// Config.hpp
std::unique_ptr<Config> Config::Create() {
	return std::make_unique<Win32RegistryKey>();
}

// Font.hpp
std::unique_ptr<Font> Font::Create(std::wstring name, int height, bool isBold) {
	return std::make_unique<Win32Font>(std::move(name), height, isBold);
}

// Utility.hpp
std::u8string EncodeToUTF8(const std::wstring_view& string) {
	const int length = WideCharToMultiByte(CP_UTF8, 0, string.data(), static_cast<int>(string.size()) + 1, nullptr, 0, nullptr, nullptr);
	std::u8string result(length - 1, 0);

	WideCharToMultiByte(CP_UTF8, 0, string.data(), static_cast<int>(string.size()) + 1, reinterpret_cast<LPSTR>(result.data()), length, nullptr, nullptr);
	return result;
}
#else
#	error 지원하지 않는 플랫폼입니다.
#endif