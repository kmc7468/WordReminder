#include "Question.hpp"

#include "Utility.hpp"

#include <algorithm>
#include <cassert>
#include <iterator>
#include <random>

QuestionType::QuestionType(Type type) noexcept
	: m_Type(type) {}

QuestionType::Type QuestionType::GetType() const noexcept {
	return m_Type;
}
void QuestionType::ExcludeUsedAnswer(const Vocabulary* vocabulary) {
	assert(!m_ExcludeUsedAnswer);

	m_ExcludeUsedAnswer = true;

	const std::size_t countOfWords = vocabulary->GetCountOfWords();
	for (std::size_t i = 0; i < countOfWords; ++i) {
		const Word* word = (*vocabulary)[i];
		auto copiedWord = std::make_unique<Word>(*word);

		const std::size_t countOfMeanings = word->GetCountOfMeanings();
		for (std::size_t j = 0; j < countOfMeanings; ++j) {
			const Meaning* meaning = (*word)[j];
			if (IsUsableMeaning(meaning)) {
				copiedWord->AddMeaning(std::make_unique<Meaning>(*meaning, copiedWord.get()));
			}
		}

		if (copiedWord->GetCountOfMeanings() > 0) {
			m_UsableVocabulary.AddWord(std::move(copiedWord));
		}
	}
}

std::wstring QuestionType::GetSelector(const Question* question, int selectorIndex) const {
	return GetFirstSelector(question, selectorIndex);
}

std::optional<Question> QuestionType::GenerateQuestion(QuestionOption* questionOption, Meaning* previousAnswer, Meaning* answer) {
	assert(!m_ExcludeUsedAnswer || answer == nullptr);

	static constexpr int loopLimit = 1000;

	switch (const auto type = GetAnswerType(); type) {
	case AnswerType::OneMultipleChoice:
	case AnswerType::TwoMultipleChoice: {
		// Step 1: 정답 선택
		if (!answer) {
			Vocabulary* vocabulary = questionOption->GetVocabulary();
			if (m_ExcludeUsedAnswer) {
				if (vocabulary->GetCountOfWords() == 0) return std::nullopt;
				else {
					vocabulary = &m_UsableVocabulary;
				}
			}

			answer = ExtractUniqueMeaning(vocabulary, { previousAnswer });
			if (!answer) return std::nullopt;
		}

		// Step 2: 선지 선택
		std::vector<Meaning*> selectors{ answer };
		for (int i = 1; i < questionOption->GetNumberOfSelectors(); ++i) {
			if (Meaning* const meaning = ExtractUniqueMeaning(questionOption->GetVocabulary(), selectors);
				meaning != nullptr) {
				selectors.push_back(meaning);
			} else return std::nullopt;
		}

		// Step 3: 정답 섞기
		const int answerIndex = GetRandomInteger(0, questionOption->GetNumberOfSelectors());
		std::iter_swap(selectors.begin(), selectors.begin() + answerIndex);

		if (type == AnswerType::TwoMultipleChoice) {
			std::vector<Meaning*> secondSelectors = selectors;
			std::shuffle(secondSelectors.begin(), secondSelectors.end(), std::mt19937(std::random_device{}()));
			const int secondAnswerIndex = static_cast<int>(std::distance(secondSelectors.begin(),
				std::find(secondSelectors.begin(), secondSelectors.end(), selectors[answerIndex])));

			return Question(this, std::move(selectors), answerIndex, std::move(secondSelectors), secondAnswerIndex);
		} else return Question(this, std::move(selectors), answerIndex);
	}

	default: return std::nullopt;
	}
}

Meaning* QuestionType::ExtractUniqueMeaning(Vocabulary* vocabulary, const std::vector<Meaning*>& fixedMeanings) const {
	for (int tryCount = 0; tryCount < 1000; ++tryCount) {
		if (Meaning* const meaning = vocabulary->ExtractRandomMeaning();
			IsUniqueMeaning(fixedMeanings, meaning)) return meaning;
	}

	std::vector<Meaning*> uniqueMeanings;

	const std::size_t countOfWords = m_UsableVocabulary.GetCountOfWords();
	for (std::size_t i = 0; i < countOfWords; ++i) {
		Word* const word = (*vocabulary)[i];

		const std::size_t countOfMeanings = word->GetCountOfMeanings();
		for (std::size_t j = 0; j < countOfMeanings; ++j) {
			if (Meaning* const meaning = (*word)[j];
				IsUniqueMeaning(fixedMeanings, meaning)) {
				uniqueMeanings.push_back(meaning);
			}
		}
	}

	if (uniqueMeanings.empty()) return nullptr;
	else return uniqueMeanings[GetRandomInteger(0, static_cast<int>(uniqueMeanings.size()))];
}
bool QuestionType::IsUniqueMeaning(const std::vector<Meaning*>& fixedMeanings, Meaning* meaning) const {
	bool isUnique = true;
	for (const auto fixedMeaning : fixedMeanings) {
		if (IsDuplicatedMeaning(fixedMeaning, meaning)) {
			isUnique = false;
			break;
		}
	}
	return isUnique;
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

Question::Question(QuestionType* type, std::vector<Meaning*>&& firstSelectors, int answerOfFirstSelectors) noexcept
	: m_Type(type), m_FirstSelectors(std::make_pair(std::move(firstSelectors), answerOfFirstSelectors)) {}
Question::Question(QuestionType* type, std::vector<Meaning*>&& firstSelectors, int answerOfFirstSelectors,
	std::vector<Meaning*>&& secondSelectors, int answerOfSecondSelectors) noexcept
	: m_Type(type), m_FirstSelectors(std::make_pair(std::move(firstSelectors), answerOfFirstSelectors)),
	m_SecondSelectors(std::make_pair(std::move(secondSelectors), answerOfSecondSelectors)) {}

const QuestionType* Question::GetType() const noexcept {
	return m_Type;
}
QuestionType* Question::GetType() noexcept {
	return m_Type;
}
const Meaning* Question::GetFirstSelector(int index) const noexcept {
	return m_FirstSelectors->first[index];
}
Meaning* Question::GetFirstSelector(int index) noexcept {
	return m_FirstSelectors->first[index];
}
int Question::GetAnswerOfFirstSelectors() const noexcept {
	return m_FirstSelectors->second;
}
const Meaning* Question::GetSecondSelector(int index) const noexcept {
	return m_SecondSelectors->first[index];
}
Meaning* Question::GetSecondSelector(int index) noexcept {
	return m_SecondSelectors->first[index];
}
int Question::GetAnswerOfSecondSelectors() const noexcept {
	return m_SecondSelectors->second;
}