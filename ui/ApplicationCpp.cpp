#include "Application.hpp"

#include "../pal/Config.hpp"

#include <cassert>
#include <utility>

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

bool Application::Initialize(std::unique_ptr<ApplicationState>&& applicationState) {
	Setting.Load();

	m_ApplicationState = std::move(applicationState);
	return m_ApplicationState->Initialize();
}
int Application::Run(std::unique_ptr<Window>&& mainWindow) {
	m_MainWindow = std::move(mainWindow);
	m_MainWindow->Show();

	return m_ApplicationState->Run();
}
void Application::Finalize() {
	Setting.Save();

	m_ApplicationState->Finalize();
}

Window* Application::GetMainWindow() noexcept {
	return m_MainWindow.get();
}

Application& Application::Get() {
	static Application instance;
	return instance;
}