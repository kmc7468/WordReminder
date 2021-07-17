#pragma once

#include "../pal/ApplicationState.hpp"
#include "../pal/Component.hpp"

#include <memory>
#include <optional>
#include <string>

#define WR_APPLICATION_VERSION L"1.4.0-beta.4"
#define WR_APPLICATION_GITHUB_RELEASE L"https://github.com/kmc7468/WordReminder/releases/latest"

class Setting final {
private:
	bool m_Loaded = false;

public: // Global
	std::wstring FontName = L"³ª´®°íµñ";
	int Scale = 100;

public: // QuestionOptionScene
	bool EnableWordToMeaning = true;
	int WordToMeaningOption = 1;
	bool EnableMeaningToWord = true;
	int MeaningToWordOption = 1;
	bool EnableWordToPronunciation = false;
	int WordToPronunciationOption = 0;

	bool ExcludeDuplicatedAnswer = false;

public:
	Setting() noexcept = default;
	Setting(const Setting&) = delete;
	~Setting() = default;

public:
	Setting& operator=(const Setting&) = delete;

public:
	void Load();
	void Save() const;
};

class Application final {
public:
	::Setting Setting;

private:
	std::unique_ptr<ApplicationState> m_ApplicationState;
	std::unique_ptr<Window> m_MainWindow;

public:
	Application() noexcept = default;
	Application(const Application&) = delete;
	~Application() = default;

public:
	Application& operator=(const Application&) = delete;

public:
	bool Initialize(std::unique_ptr<ApplicationState>&& applicationState);
	int Run(std::unique_ptr<Window>&& mainWindow);
	void Finalize();

	template<typename T>
	T* GetApplicationState() noexcept {
		return static_cast<T*>(m_ApplicationState.get());
	}
	Window* GetMainWindow() noexcept;

public:
	static Application& Get();
};