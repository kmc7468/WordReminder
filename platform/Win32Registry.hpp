#pragma once

#include "Win32API.hpp"

#include "../pal/Config.hpp"

#include <filesystem>
#include <optional>
#include <string>

class Win32RegistryKey final : public Config {
public:
	static const Win32RegistryKey CurrentUser;

private:
	HKEY m_Key = nullptr;
	bool m_CloseKey = true;

public:
	Win32RegistryKey() noexcept = default;
	Win32RegistryKey(const Win32RegistryKey&) = delete;
	Win32RegistryKey(Win32RegistryKey&& registry) noexcept;
	virtual ~Win32RegistryKey() override;

private:
	Win32RegistryKey(HKEY key) noexcept;

public:
	Win32RegistryKey& operator=(const Win32RegistryKey&) = delete;
	Win32RegistryKey& operator=(Win32RegistryKey&& registryKey) noexcept;

public:
	virtual bool IsOpen() const noexcept override;
	virtual bool Open(const std::filesystem::path& path) override;
	bool Open(const Win32RegistryKey& key, const std::wstring& subKey) noexcept;
	virtual void Close() override;

	virtual std::optional<int> ReadInt(const std::wstring& valueName) const override;
	virtual bool WriteInt(const std::wstring& valueName, int value) override;
	virtual std::optional<std::wstring> ReadString(const std::wstring& valueName) const override;
	virtual bool WriteString(const std::wstring& valueName, const std::wstring& value) override;
	virtual void Delete(const std::wstring& valueName) override;

private:
	bool QueryValue(const std::wstring& valueName, DWORD dataType, void* buffer, LPDWORD bufferSize) const noexcept;
	bool SetValue(const std::wstring& valueName, DWORD dataType, const void* buffer, std::size_t bufferSize) noexcept;
};