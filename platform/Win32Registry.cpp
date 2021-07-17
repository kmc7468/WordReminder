#include "Win32Registry.hpp"

#include <cassert>

const Win32RegistryKey Win32RegistryKey::CurrentUser(HKEY_CURRENT_USER);

Win32RegistryKey::Win32RegistryKey(Win32RegistryKey&& registry) noexcept
	: m_Key(registry.m_Key), m_CloseKey(registry.m_CloseKey) {
	registry.m_Key = nullptr;
}
Win32RegistryKey::~Win32RegistryKey() {
	if (m_CloseKey) {
		Close();
	}
}

Win32RegistryKey::Win32RegistryKey(HKEY key) noexcept
	: m_Key(key), m_CloseKey(false) {}

Win32RegistryKey& Win32RegistryKey::operator=(Win32RegistryKey&& registryKey) noexcept {
	Close();

	m_Key = registryKey.m_Key;
	m_CloseKey = registryKey.m_CloseKey;

	registryKey.m_Key = nullptr;
	return *this;
}

bool Win32RegistryKey::IsOpen() const noexcept {
	return m_Key != nullptr;
}
bool Win32RegistryKey::Open(const std::filesystem::path& path) {
	return Open(CurrentUser, L"SOFTWARE\\" + path.wstring());
}
bool Win32RegistryKey::Open(const Win32RegistryKey& key, const std::wstring& subKey) noexcept {
	assert(m_Key == nullptr);
	assert(&key != this);

	return RegOpenKeyW(key.m_Key, subKey.data(), &m_Key) == ERROR_SUCCESS;
}
void Win32RegistryKey::Close() {
	if (m_Key) {
		RegCloseKey(m_Key);
	}
}

std::optional<int> Win32RegistryKey::ReadInt(const std::wstring& valueName) const {
	DWORD buffer;
	DWORD bufferSize = sizeof(buffer);
	if (QueryValue(valueName, REG_DWORD, &buffer, &bufferSize)) return static_cast<int>(buffer);
	else return std::nullopt;
}
bool Win32RegistryKey::WriteInt(const std::wstring& valueName, int value) {
	return SetValue(valueName, REG_DWORD, &value, sizeof(value));
}
std::optional<std::wstring> Win32RegistryKey::ReadString(const std::wstring& valueName) const {
	DWORD bufferSize;
	if (!QueryValue(valueName, REG_SZ, nullptr, &bufferSize)) return std::nullopt;

	std::wstring buffer(bufferSize - sizeof(wchar_t), 0);
	if (QueryValue(valueName, REG_SZ, buffer.data(), &bufferSize)) return buffer;
	else return std::nullopt;
}
bool Win32RegistryKey::WriteString(const std::wstring& valueName, const std::wstring& value) {
	return SetValue(valueName, REG_SZ, value.data(), value.size() * sizeof(wchar_t));
}
void Win32RegistryKey::Delete(const std::wstring& valueName) {
	RegDeleteKeyW(m_Key, valueName.data());
}

bool Win32RegistryKey::QueryValue(const std::wstring& valueName, DWORD dataType, void* buffer, LPDWORD bufferSize) const noexcept {
	assert(m_Key != nullptr);
	assert(!valueName.empty());

	DWORD realDataType;
	return RegQueryValueExW(m_Key, valueName.data(), nullptr, &realDataType, static_cast<LPBYTE>(buffer), bufferSize) == ERROR_SUCCESS &&
		realDataType == dataType;
}
bool Win32RegistryKey::SetValue(const std::wstring& valueName, DWORD dataType, const void* buffer, std::size_t bufferSize) noexcept {
	assert(m_Key != nullptr);
	assert(!valueName.empty());

	return RegSetValueExW(m_Key, valueName.data(), 0, dataType, static_cast<LPCBYTE>(buffer), static_cast<DWORD>(bufferSize)) == ERROR_SUCCESS;
}