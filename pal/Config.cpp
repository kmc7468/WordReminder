#include "Config.hpp"

#include "../platform/Win32.hpp"

std::optional<bool> Config::ReadBool(const std::wstring& name) const {
	if (const auto result = ReadInt(name); result) return !!*result;
	else return std::nullopt;
}
bool Config::WriteBool(const std::wstring& name, bool value) {
	return WriteInt(name, value);
}

std::unique_ptr<Config> Config::Create() {
	return std::make_unique<RegistryKey>();
}