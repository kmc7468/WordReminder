#include "Config.hpp"

std::optional<bool> Config::ReadBool(const std::wstring& name) const {
	if (const auto result = ReadInt(name); result) return !!*result;
	else return std::nullopt;
}
bool Config::WriteBool(const std::wstring& name, bool value) {
	return WriteInt(name, value);
}