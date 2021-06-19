#pragma once

#include "Vocabulary.hpp"

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

class Question;

class QuestionType {
public:
	enum Id {
		WordToMeaning,
		MeaningToWord,
		WordToPronunciation,
	};
	enum InputType {
		OneSelector,
		TwoSelector,
	};

private:
	Id m_Id;

protected:
	QuestionType(Id id) noexcept;

public:
	QuestionType(const QuestionType&) = delete;
	virtual ~QuestionType() = default;

public:
	QuestionType& operator=(const QuestionType&) = delete;

public:
	Id GetId() const noexcept;
	virtual InputType GetInputType() const noexcept = 0;

	virtual std::wstring GetQuestionDescription() const = 0;
	virtual std::wstring GetQuestion(const Question* question) const = 0;
	virtual std::wstring GetHint(const Question* question) const = 0;
	std::wstring GetSelector(const Question* question, int selectorIndex) const;
	virtual std::wstring GetFirstSelector(const Question* question, int selectorIndex) const = 0;
	virtual std::wstring GetSecondSelector(const Question* question, int selectorIndex) const = 0;
};

class QuestionOption final {
private:
	std::unique_ptr<Vocabulary> m_Vocabulary;
	std::vector<std::unique_ptr<QuestionType>> m_QuestionTypes;
	int m_NumberOfSelectors = 5;

	bool m_ExcludeDuplicatedAnswer = false;

public:
	QuestionOption(int numberOfSelectors) noexcept;
	QuestionOption(const QuestionOption&) = delete;
	QuestionOption(QuestionOption&& questionOption) noexcept = default;
	~QuestionOption() = default;

public:
	QuestionOption& operator=(const QuestionOption&) = delete;
	QuestionOption& operator=(QuestionOption&& questionOption) noexcept = default;

public:
	const Vocabulary* GetVocabulary() const noexcept;
	Vocabulary* GetVocabulary() noexcept;
	void SetVocabulary(std::unique_ptr<Vocabulary>&& newVocabulary) noexcept;
	const QuestionType* GetQuestionType(std::size_t index) const noexcept;
	QuestionType* GetQuestionType(std::size_t index) noexcept;
	std::size_t GetCountOfQuestionTypes() const noexcept;
	void AddQuestionType(std::unique_ptr<QuestionType>&& questionType);
	int GetNumberOfSelectors() const noexcept;

	bool GetExcludeDuplicatedAnswer() const noexcept;
	void SetExcludeDuplicatedAnswer(bool newExcludeDuplicatedAnswer) noexcept;
};

class Question final {
private:
	QuestionType* m_Type = nullptr;
	std::vector<Meaning*> m_FirstSelectors;
	int m_AnswerOfFirstSelectors;
	std::vector<Meaning*> m_SecondSelectors;
	int m_AnswerOfSecondSelectors;

public:
	Question(QuestionType* type, std::vector<Meaning*>&& firstSelectors, int answerOfFirstSelectors,
		std::vector<Meaning*>&& secondSelectors, int answerOfSecondSelectors) noexcept;
	Question(const Question&) = delete;
	Question(Question&& question) noexcept = default;
	~Question() = default;

public:
	Question& operator=(const Question&) = delete;
	Question& operator=(Question&& question) noexcept = default;

public:
	const QuestionType* GetType() const noexcept;
	QuestionType* GetType() noexcept;
	const Meaning* GetFirstSelector(int index) const noexcept;
	Meaning* GetFirstSelector(int index) noexcept;
	int GetAnswerOfFirstSelectors() const noexcept;
	const Meaning* GetSecondSelector(int index) const noexcept;
	Meaning* GetSecondSelector(int index) noexcept;
	int GetAnswerOfSecondSelectors() const noexcept;
};