#pragma once

#include "Win32API.hpp"

#include <optional>
#include <string>

class RegistryKey final {
public:
	static const RegistryKey CurrentUser;

private:
	HKEY m_Key = nullptr;
	bool m_CloseKey = true;

public:
	RegistryKey() noexcept = default;
	RegistryKey(const RegistryKey&) = delete;
	RegistryKey(RegistryKey&& registry) noexcept;
	~RegistryKey();

private:
	RegistryKey(HKEY key) noexcept;

public:
	RegistryKey& operator=(const RegistryKey&) = delete;
	RegistryKey& operator=(RegistryKey&& registryKey) noexcept;

public:
	bool IsOpen() const noexcept;
	bool Open(const RegistryKey& key, const std::wstring& subKey) noexcept;
	void Close() noexcept;

	std::optional<bool> ReadBool(const std::wstring& valueName) const noexcept;
	bool WriteBool(const std::wstring& valueName, bool value) noexcept;
	std::optional<int> ReadInt(const std::wstring& valueName) const noexcept;
	bool WriteInt(const std::wstring& valueName, int value) noexcept;
	std::optional<std::wstring> ReadString(const std::wstring& valueName) const;
	bool WriteString(const std::wstring& valueName, const std::wstring& value) noexcept;
	void Delete(const std::wstring& valueName) noexcept;

private:
	bool QueryValue(const std::wstring& valueName, DWORD dataType, void* buffer, LPDWORD bufferSize) const noexcept;
	bool SetValue(const std::wstring& valueName, DWORD dataType, const void* buffer, std::size_t bufferSize) noexcept;
};