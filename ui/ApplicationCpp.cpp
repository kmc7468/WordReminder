#include "Application.hpp"

#include "../pal/Config.hpp"

#include <cassert>

void Setting::Load() {
	assert(!m_Loaded);

	const auto config = Config::Create();
	if (!config->Open(L"Staticom\\WordReminder")) return;

#define Concat(a, b) a##b
#define Read(name, type) name = config->Concat(Read, type)(L#name).value_or(name)

	// Global
	Read(FontName, String);
	Read(Scale, Int);

	// QuestionOptionScene
	Read(EnableWordToMeaning, Bool);
	Read(WordToMeaningOption, Int);
	Read(EnableMeaningToWord, Bool);
	Read(MeaningToWordOption, Int);
	Read(EnableWordToPronunciation, Bool);
	Read(WordToPronunciationOption, Int);

	Read(ExcludeDuplicatedAnswer, Bool);
}
void Setting::Save() const {
	const auto config = Config::Create();
	if (!config->Open(L"Staticom\\WordReminder")) return;

#define Write(name, type) config->Concat(Write, type)(L#name, name)

	// Global
	Write(FontName, String);
	Write(Scale, Int);

	// QuestionOptionScene
	Write(EnableWordToMeaning, Bool);
	Write(WordToMeaningOption, Int);
	Write(EnableMeaningToWord, Bool);
	Write(MeaningToWordOption, Int);
	Write(EnableWordToPronunciation, Bool);
	Write(WordToPronunciationOption, Int);

	Write(ExcludeDuplicatedAnswer, Bool);
}