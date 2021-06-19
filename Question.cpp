#include "Question.hpp"

#include <cassert>

QuestionType::QuestionType(Id id) noexcept
	: m_Id(id) {}

QuestionType::Id QuestionType::GetId() const noexcept {
	return m_Id;
}

std::wstring QuestionType::GetSelector(const Question* question, int selectorIndex) const {
	return GetFirstSelector(question, selectorIndex);
}

QuestionOption::QuestionOption(int numberOfSelectors) noexcept
	: m_NumberOfSelectors(numberOfSelectors) {
	assert(numberOfSelectors > 0);
}

const Vocabulary* QuestionOption::GetVocabulary() const noexcept {
	return m_Vocabulary.get();
}
Vocabulary* QuestionOption::GetVocabulary() noexcept {
	return m_Vocabulary.get();
}
void QuestionOption::SetVocabulary(std::unique_ptr<Vocabulary>&& newVocabulary) noexcept {
	m_Vocabulary = std::move(newVocabulary);
}
const QuestionType* QuestionOption::GetQuestionType(std::size_t index) const noexcept {
	return m_QuestionTypes[index].get();
}
QuestionType* QuestionOption::GetQuestionType(std::size_t index) noexcept {
	return m_QuestionTypes[index].get();
}
std::size_t QuestionOption::GetCountOfQuestionTypes() const noexcept {
	return m_QuestionTypes.size();
}
void QuestionOption::AddQuestionType(std::unique_ptr<QuestionType>&& questionType) {
	m_QuestionTypes.push_back(std::move(questionType));
}
int QuestionOption::GetNumberOfSelectors() const noexcept {
	return m_NumberOfSelectors;
}

bool QuestionOption::GetExcludeDuplicatedAnswer() const noexcept {
	return m_ExcludeDuplicatedAnswer;
}
void QuestionOption::SetExcludeDuplicatedAnswer(bool newExcludeDuplicatedAnswer) noexcept {
	m_ExcludeDuplicatedAnswer = newExcludeDuplicatedAnswer;
}