#pragma once

#include <filesystem>
#include <memory>
#include <optional>
#include <string>

class Config {
public:
	Config() noexcept = default;
	Config(const Config&) = delete;
	virtual ~Config() = default;

public:
	Config& operator=(const Config&) = delete;

public:
	virtual bool IsOpen() const noexcept = 0;
	virtual bool Open(const std::filesystem::path& path) = 0;
	virtual void Close() = 0;

	virtual std::optional<bool> ReadBool(const std::wstring& name) const;
	virtual bool WriteBool(const std::wstring& name, bool value);
	virtual std::optional<int> ReadInt(const std::wstring& name) const = 0;
	virtual bool WriteInt(const std::wstring& name, int value) = 0;
	virtual std::optional<std::wstring> ReadString(const std::wstring& name) const = 0;
	virtual bool WriteString(const std::wstring& name, const std::wstring& value) = 0;
	virtual void Delete(const std::wstring& name) = 0;

public:
	static std::unique_ptr<Config> Create();
};