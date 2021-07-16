#include "QuestionTypes.hpp"

#include <cassert>
#include <sstream>

WordToMeaning::WordToMeaning(bool enableHint, bool enableSecondSelectors) noexcept
	: QuestionType(Type::WordToMeaning), m_EnableHint(enableHint), m_EnableSecondSelectors(enableSecondSelectors) {
	assert(!enableHint || !enableSecondSelectors);
}

WordToMeaning::AnswerType WordToMeaning::GetAnswerType() const noexcept {
	return m_EnableSecondSelectors ? AnswerType::TwoMultipleChoice : AnswerType::OneMultipleChoice;
}

std::wstring WordToMeaning::GetQuestionDescription() const {
	return L"다음 단어의 뜻은?";
}
std::wstring WordToMeaning::GetQuestion(const Question* question) const {
	return std::wstring(question->GetFirstSelector(question->GetAnswerOfFirstSelectors())->GetWord()->GetWord());
}
std::wstring WordToMeaning::GetHint(const Question* question) const {
	return std::wstring(m_EnableHint ? question->GetFirstSelector(question->GetAnswerOfFirstSelectors())->GetPronunciation() : L"");
}
std::wstring WordToMeaning::GetFirstSelector(const Question* question, int selectorIndex) const {
	const Meaning* selector = question->GetFirstSelector(selectorIndex);

	std::wostringstream oss;
	oss << selector->GetMeaning();

	if (m_EnableHint && selector->HasPronunciation()) {
		oss << L"\n(" << selector->GetPronunciation() << ')';
	}

	return oss.str();
}
std::wstring WordToMeaning::GetSecondSelector(const Question* question, int selectorIndex) const {
	return std::wstring(question->GetFirstSelector(selectorIndex)->GetPronunciation());
}

bool WordToMeaning::IsUsableMeaning(const Meaning* meaning) const {
	return m_EnableSecondSelectors ? meaning->HasPronunciation() : true;
}
bool WordToMeaning::IsDuplicatedMeaning(const Meaning* fixedMeaning, const Meaning* targetMeaning) const {
	return fixedMeaning->GetWord() == targetMeaning->GetWord() ||
		fixedMeaning->GetMeaning() == targetMeaning->GetMeaning() ||
		(m_EnableSecondSelectors ? fixedMeaning->GetPronunciation() == targetMeaning->GetPronunciation() : false);
}

MeaningToWord::MeaningToWord(bool enableHint, bool enableSecondSelectors) noexcept
	: QuestionType(Type::MeaningToWord), m_EnableHint(enableHint), m_EnableSecondSelectors(enableSecondSelectors) {
	assert(!enableHint || !enableSecondSelectors);
}

MeaningToWord::AnswerType MeaningToWord::GetAnswerType() const noexcept {
	return m_EnableSecondSelectors ? AnswerType::TwoMultipleChoice : AnswerType::OneMultipleChoice;
}

std::wstring MeaningToWord::GetQuestionDescription() const {
	return L"다음 뜻을 가진 단어는?";
}
std::wstring MeaningToWord::GetQuestion(const Question* question) const {
	return std::wstring(question->GetFirstSelector(question->GetAnswerOfFirstSelectors())->GetMeaning());
}
std::wstring MeaningToWord::GetHint(const Question* question) const {
	return std::wstring(m_EnableHint ? question->GetFirstSelector(question->GetAnswerOfFirstSelectors())->GetPronunciation() : L"");
}
std::wstring MeaningToWord::GetFirstSelector(const Question* question, int selectorIndex) const {
	const Meaning* selector = question->GetFirstSelector(selectorIndex);

	std::wostringstream oss;
	oss << selector->GetWord()->GetWord();

	if (m_EnableHint && selector->HasPronunciation()) {
		oss << L"\n(" << selector->GetPronunciation() << ')';
	}

	return oss.str();
}
std::wstring MeaningToWord::GetSecondSelector(const Question* question, int selectorIndex) const {
	return std::wstring(question->GetFirstSelector(selectorIndex)->GetPronunciation());
}

bool MeaningToWord::IsUsableMeaning(const Meaning* meaning) const {
	return m_EnableSecondSelectors ? meaning->HasPronunciation() : true;
}
bool MeaningToWord::IsDuplicatedMeaning(const Meaning* fixedMeaning, const Meaning* targetMeaning) const {
	return fixedMeaning->GetWord() == targetMeaning->GetWord() ||
		fixedMeaning->GetMeaning() == targetMeaning->GetMeaning() ||
		(m_EnableSecondSelectors ? fixedMeaning->GetPronunciation() == targetMeaning->GetPronunciation() : false);
}

WordToPronunciation::WordToPronunciation(bool enableHint) noexcept
	: QuestionType(Type::WordToPronunciation), m_EnableHint(enableHint) {}

WordToPronunciation::AnswerType WordToPronunciation::GetAnswerType() const noexcept {
	return AnswerType::OneMultipleChoice;
}

std::wstring WordToPronunciation::GetQuestionDescription() const {
	return L"다음 단어의 발음은?";
}
std::wstring WordToPronunciation::GetQuestion(const Question* question) const {
	return std::wstring(question->GetFirstSelector(question->GetAnswerOfFirstSelectors())->GetWord()->GetWord());
}
std::wstring WordToPronunciation::GetHint(const Question* question) const {
	return std::wstring(m_EnableHint ? question->GetFirstSelector(question->GetAnswerOfFirstSelectors())->GetMeaning() : L"");
}
std::wstring WordToPronunciation::GetFirstSelector(const Question* question, int selectorIndex) const {
	return std::wstring(question->GetFirstSelector(selectorIndex)->GetPronunciation());
}

bool WordToPronunciation::IsUsableMeaning(const Meaning* meaning) const {
	return meaning->HasPronunciation();
}
bool WordToPronunciation::IsDuplicatedMeaning(const Meaning* fixedMeaning, const Meaning* targetMeaning) const {
	return fixedMeaning->GetWord() == targetMeaning->GetWord() ||
		fixedMeaning->GetPronunciation() == targetMeaning->GetPronunciation();
}