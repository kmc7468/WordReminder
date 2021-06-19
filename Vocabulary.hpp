#pragma once

#include <cstddef>
#include <fstream>
#include <ios>
#include <memory>
#include <string>
#include <vector>

class Word;

class Meaning final {
private:
	Word* m_Word;
	std::wstring m_Meaning;
	std::wstring m_Pronunciation;

public:
	Meaning() noexcept = default;
	Meaning(Word* word, std::wstring meaning, std::wstring pronunciation) noexcept;
	Meaning(const Meaning& meaning) = default;
	Meaning(Meaning&& meaning) noexcept = default;
	~Meaning() = default;

public:
	Meaning& operator=(const Meaning& meaning) = default;
	Meaning& operator=(Meaning&& meaning) noexcept = default;

public:
	const Word* GetWord() const noexcept;
	Word* GetWord() noexcept;
	std::wstring GetMeaning() const;
	std::wstring GetPronunciation() const;
	bool HasPronunciation() const noexcept;
};

class Word final {
private:
	std::wstring m_Word;
	std::vector<std::unique_ptr<Meaning>> m_Meanings;

public:
	Word() noexcept = default;
	Word(std::wstring word) noexcept;
	Word(std::wstring word, std::wstring meaning, std::wstring pronunciation);
	Word(const Word& word);
	Word(Word&& word) noexcept = default;
	~Word() = default;

public:
	Word& operator=(const Word& word);
	Word& operator=(Word&& word) noexcept = default;
	const Meaning* operator[](std::size_t index) const noexcept;
	Meaning* operator[](std::size_t index) noexcept;

public:
	std::wstring GetWord() const;
	std::size_t GetCountOfMeanings() const noexcept;
	void AddMeaning(std::unique_ptr<Meaning>&& meaning);
	void RemoveMeaning(std::size_t index);
	std::size_t GetIndexOfMeaning(const std::wstring& meaning) const noexcept;
	const Meaning* FindMeaning(const std::wstring& meaning) const noexcept;
	Meaning* FindMeaning(const std::wstring& meaning) noexcept;

	Meaning MergeMeaning(const std::wstring& separator);

private:
	void CopyFrom(const Word& word);
};

class Vocabulary final {
public:
	enum Type {
		Kv,
		Csv,
		CsvS,
	};

private:
	static constexpr int m_HomonymContainerId = 0x00000000;

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

public:
	std::size_t GetCountOfWords() const noexcept;
	void AddWord(std::unique_ptr<Word>&& word);
	void RemoveWord(std::size_t index);
	std::size_t GetIndexOfWord(const std::wstring& word) const noexcept;
	const Word* FindWord(const std::wstring& word) const noexcept;
	Word* FindWord(const std::wstring& word) noexcept;

	bool Load(const std::wstring& path);
	bool Save(const std::wstring& path, Type type = Kv) const;

private:
	void CopyFrom(const Vocabulary& vocabulary);

	void LoadKv(std::ifstream& file);
	void SaveAsKv(std::ofstream& file) const;
	void SaveAsCsv(std::ofstream& file, bool insertBOM) const;

	std::streampos WriteContainerHeader(std::ofstream& file, int id) const;
	void WriteContainerLength(std::ofstream& file, std::streampos lengthPos) const;
	void ReadHomonymContainer(std::ifstream& file);
	void WriteHomonymContainer(std::ofstream& file) const;
};