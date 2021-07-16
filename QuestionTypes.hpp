#pragma once

#include "Question.hpp"
#include "Vocabulary.hpp"

#include <string>

class WordToMeaning final : public QuestionType {
private:
	bool m_EnableHint, m_EnableSecondSelectors;

public:
	WordToMeaning(bool enableHint, bool enableSecondSelectors) noexcept;
	WordToMeaning(const WordToMeaning&) = delete;
	virtual ~WordToMeaning() override = default;

public:
	WordToMeaning& operator=(const WordToMeaning&) = delete;

public:
	virtual AnswerType GetAnswerType() const noexcept override;

	virtual std::wstring GetQuestionDescription() const override;
	virtual std::wstring GetQuestion(const Question* question) const override;
	virtual std::wstring GetHint(const Question* question) const override;
	virtual std::wstring GetFirstSelector(const Question* question, int selectorIndex) const override;
	virtual std::wstring GetSecondSelector(const Question* question, int selectorIndex) const override;

protected:
	virtual bool IsUsableMeaning(const Meaning* meaning) const override;
	virtual bool IsDuplicatedMeaning(const Meaning* fixedMeaning, const Meaning* targetMeaning) const override;
};

class MeaningToWord final : public QuestionType {
private:
	bool m_EnableHint, m_EnableSecondSelectors;

public:
	MeaningToWord(bool enableHint, bool enableSecondSelectors) noexcept;
	MeaningToWord(const MeaningToWord&) = delete;
	virtual ~MeaningToWord() override = default;

public:
	MeaningToWord& operator=(const MeaningToWord&) = delete;

public:
	virtual AnswerType GetAnswerType() const noexcept override;

	virtual std::wstring GetQuestionDescription() const override;
	virtual std::wstring GetQuestion(const Question* question) const override;
	virtual std::wstring GetHint(const Question* question) const override;
	virtual std::wstring GetFirstSelector(const Question* question, int selectorIndex) const override;
	virtual std::wstring GetSecondSelector(const Question* question, int selectorIndex) const override;

protected:
	virtual bool IsUsableMeaning(const Meaning* meaning) const override;
	virtual bool IsDuplicatedMeaning(const Meaning* fixedMeaning, const Meaning* targetMeaning) const override;
};

class WordToPronunciation final : public QuestionType {
private:
	bool m_EnableHint;

public:
	WordToPronunciation(bool enableHint) noexcept;
	WordToPronunciation(const WordToPronunciation&) = delete;
	virtual ~WordToPronunciation() override = default;

public:
	WordToPronunciation& operator=(const WordToPronunciation&) = delete;

public:
	virtual AnswerType GetAnswerType() const noexcept override;

	virtual std::wstring GetQuestionDescription() const override;
	virtual std::wstring GetQuestion(const Question* question) const override;
	virtual std::wstring GetHint(const Question* question) const override;
	virtual std::wstring GetFirstSelector(const Question* question, int selectorIndex) const override;

protected:
	virtual bool IsUsableMeaning(const Meaning* meaning) const override;
	virtual bool IsDuplicatedMeaning(const Meaning* fixedMeaning, const Meaning* targetMeaning) const override;
};