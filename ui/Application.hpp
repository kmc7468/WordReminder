#pragma once

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