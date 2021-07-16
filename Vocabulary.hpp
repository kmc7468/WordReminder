#pragma once

#include <cstddef>
#include <fstream>
#include <ios>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

class Word;

class Meaning final {
private:
	Word* m_Word;
	std::wstring m_Meaning;
	std::wstring m_Pronunciation;

public:
	Meaning(Word* word, std::wstring meaning, std::wstring pronunciation) noexcept;
	Meaning(const Meaning& meaning) = default;
	Meaning(const Meaning& meaning, Word* newWord);
	Meaning(Meaning&& meaning) noexcept = default;
	Meaning(Meaning&& meaning, Word* newWord) noexcept;
	~Meaning() = default;

public:
	Meaning& operator=(const Meaning& meaning) = default;
	Meaning& operator=(Meaning&& meaning) noexcept = default;

public:
	const Word* GetWord() const noexcept;
	Word* GetWord() noexcept;
	void SetWord(Word* newWord) noexcept;
	std::wstring_view GetMeaning() const noexcept;
	std::wstring_view GetPronunciation() const noexcept;
	bool HasPronunciation() const noexcept;
};

class Word final {
private:
	std::wstring m_Word;
	std::vector<std::unique_ptr<Meaning>> m_Meanings;

public:
	Word(std::wstring word) noexcept;
	Word(std::wstring word, std::wstring meaning, std::wstring pronunciation);
	Word(const Word& word);
	Word(Word&& word) noexcept;
	~Word() = default;

public:
	Word& operator=(const Word& word);
	Word& operator=(Word&& word) noexcept;
	const Meaning* operator[](std::size_t index) const noexcept;
	Meaning* operator[](std::size_t index) noexcept;

public:
	std::wstring_view GetWord() const noexcept;
	std::size_t GetCountOfMeanings() const noexcept;
	void AddMeaning(std::unique_ptr<Meaning>&& meaning);
	void RemoveMeaning(std::size_t index);
	std::size_t GetIndexOfMeaning(const std::wstring& meaning) const noexcept;
	const Meaning* FindMeaning(const std::wstring& meaning) const noexcept;
	Meaning* FindMeaning(const std::wstring& meaning) noexcept;

	Meaning MergeMeaning(const std::wstring& separator);

private:
	void CopyFrom(const Word& word);
	void MoveFrom(Word&& word) noexcept;
};

class Vocabulary final {
public:
	enum ExportType {
		Csv,
		CsvS,
	};

private:
	enum ContainerId {
		HomonymContainer,
	};

private:
	std::vector<std::unique_ptr<Word>> m_Words;

public:
	Vocabulary() noexcept = default;
	Vocabulary(const Vocabulary& vocabulary);
	Vocabulary(Vocabulary&& vocabulary) noexcept = default;
	~Vocabulary() = default;

public:
	Vocabulary& operator=(const Vocabulary& vocabulary);
	Vocabulary& operator=(Vocabulary&& vocabulary) noexcept = default;
	const Word* operator[](std::size_t index) const noexcept;
	Word* operator[](std::size_t index) noexcept;

public:
	std::size_t GetCountOfWords() const noexcept;
	void AddWord(std::unique_ptr<Word>&& word);
	void RemoveWord(std::size_t index);
	std::size_t GetIndexOfWord(const std::wstring& word) const noexcept;
	const Word* FindWord(const std::wstring& word) const noexcept;
	Word* FindWord(const std::wstring& word) noexcept;
	Meaning* ExtractRandomMeaning() noexcept;

	bool Load(const std::wstring& path);
	bool Save(const std::wstring& path) const;
	bool Export(const std::wstring& path, ExportType exportType) const;

private:
	void CopyFrom(const Vocabulary& vocabulary);

	void LoadFromStream(std::ifstream& file);
	void SaveToStream(std::ofstream& file) const;

	std::streampos WriteContainerHeader(std::ofstream& file, ContainerId id) const;
	void WriteContainerLength(std::ofstream& file, std::streampos lengthPos) const;
	void ReadHomonymContainer(std::ifstream& file);
	void WriteHomonymContainer(std::ofstream& file) const;

	void ExportAsCsv(std::ofstream& file, bool insertBOM) const;
};