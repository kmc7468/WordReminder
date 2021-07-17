#pragma once

#include "Win32API.hpp"

#include "../pal/Config.hpp"

#include <filesystem>
#include <optional>
#include <string>

class RegistryKey final : public Config {
public:
	static const RegistryKey CurrentUser;

private:
	HKEY m_Key = nullptr;
	bool m_CloseKey = true;

public:
	RegistryKey() noexcept = default;
	RegistryKey(const RegistryKey&) = delete;
	RegistryKey(RegistryKey&& registry) noexcept;
	virtual ~RegistryKey() override;

private:
	RegistryKey(HKEY key) noexcept;

public:
	RegistryKey& operator=(const RegistryKey&) = delete;
	RegistryKey& operator=(RegistryKey&& registryKey) noexcept;

public:
	virtual bool IsOpen() const noexcept override;
	virtual bool Open(const std::filesystem::path& path) override;
	bool Open(const RegistryKey& key, const std::wstring& subKey) noexcept;
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