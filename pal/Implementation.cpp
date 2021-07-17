#include "Config.hpp"

#include <memory>

#ifdef _WIN32
#	include "../platform/Win32.hpp"

std::unique_ptr<Config> Config::Create() {
	return std::make_unique<RegistryKey>();
}
#else
#	error �������� �ʴ� �÷����Դϴ�.
#endif