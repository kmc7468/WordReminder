#include "Question.hpp"

#include <cassert>
#include <utility>

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

Question::Question(QuestionType* type, std::vector<Meaning*>&& firstSelectors, int answerOfFirstSelectors,
	std::vector<Meaning*>&& secondSelectors, int answerOfSecondSelectors) noexcept
	: m_Type(type), m_FirstSelectors(std::move(firstSelectors)), m_AnswerOfFirstSelectors(answerOfFirstSelectors),
	m_SecondSelectors(std::move(secondSelectors)), m_AnswerOfSecondSelectors(answerOfSecondSelectors) {}

const QuestionType* Question::GetType() const noexcept {
	return m_Type;
}
QuestionType* Question::GetType() noexcept {
	return m_Type;
}
const Meaning* Question::GetFirstSelector(int index) const noexcept {
	return m_FirstSelectors[index];
}
Meaning* Question::GetFirstSelector(int index) noexcept {
	return m_FirstSelectors[index];
}
int Question::GetAnswerOfFirstSelectors() const noexcept {
	return m_AnswerOfFirstSelectors;
}
const Meaning* Question::GetSecondSelector(int index) const noexcept {
	return m_SecondSelectors[index];
}
Meaning* Question::GetSecondSelector(int index) noexcept {
	return m_SecondSelectors[index];
}
int Question::GetAnswerOfSecondSelectors() const noexcept {
	return m_AnswerOfSecondSelectors;
}