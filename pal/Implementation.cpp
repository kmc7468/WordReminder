#include "Config.hpp"

#include <memory>

#ifdef _WIN32
#	include "../platform/Win32.hpp"

std::unique_ptr<Config> Config::Create() {
	return std::make_unique<RegistryKey>();
}
#else
#	error 지원하지 않는 플랫폼입니다.
#endif