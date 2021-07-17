#include "Vocabulary.hpp"

#include "Utility.hpp"

#include "../pal/Utility.hpp"

#include <algorithm>
#include <cassert>
#include <sstream>
#include <unordered_set>
#include <utility>

Meaning::Meaning(Word* word, std::wstring meaning, std::wstring pronunciation) noexcept
	: m_Word(word), m_Meaning(std::move(meaning)), m_Pronunciation(std::move(pronunciation)) {
	assert(word != nullptr);
	assert(!m_Meaning.empty());
}
Meaning::Meaning(const Meaning& meaning, Word* newWord)
	: m_Word(newWord), m_Meaning(meaning.m_Meaning), m_Pronunciation(meaning.m_Pronunciation) {
	assert(newWord != nullptr);
}
Meaning::Meaning(Meaning&& meaning, Word* newWord) noexcept
	: m_Word(newWord), m_Meaning(std::move(meaning.m_Meaning)), m_Pronunciation(std::move(meaning.m_Pronunciation)) {
	assert(newWord != nullptr);
}

const Word* Meaning::GetWord() const noexcept {
	return m_Word;
}
Word* Meaning::GetWord() noexcept {
	return m_Word;
}
void Meaning::SetWord(Word* newWord) noexcept {
	assert(newWord != nullptr);
	m_Word = newWord;
}
std::wstring_view Meaning::GetMeaning() const noexcept {
	return m_Meaning;
}
std::wstring_view Meaning::GetPronunciation() const noexcept {
	return m_Pronunciation;
}
bool Meaning::HasPronunciation() const noexcept {
	return !m_Pronunciation.empty() && m_Word->GetWord() != m_Pronunciation;
}

Word::Word(std::wstring word) noexcept
	: m_Word(std::move(word)) {}
Word::Word(std::wstring word, std::wstring meaning, std::wstring pronunciation)
	: m_Word(std::move(word)) {
	m_Meanings.push_back(std::make_unique<Meaning>(this, std::move(meaning), std::move(pronunciation)));
}
Word::Word(const Word& word) {
	CopyFrom(word);
}
Word::Word(Word&& word) noexcept {
	MoveFrom(std::move(word));
}

Word& Word::operator=(const Word& word) {
	CopyFrom(word);
	return *this;
}
Word& Word::operator=(Word&& word) noexcept {
	MoveFrom(std::move(word));
	return *this;
}
const Meaning* Word::operator[](std::size_t index) const noexcept {
	return m_Meanings[index].get();
}
Meaning* Word::operator[](std::size_t index) noexcept {
	return m_Meanings[index].get();
}

std::wstring_view Word::GetWord() const noexcept {
	return m_Word;
}
std::size_t Word::GetCountOfMeanings() const noexcept {
	return m_Meanings.size();
}
void Word::AddMeaning(std::unique_ptr<Meaning>&& meaning) {
	m_Meanings.push_back(std::move(meaning));
}
void Word::RemoveMeaning(std::size_t index) {
	m_Meanings.erase(m_Meanings.begin() + index);
}
std::size_t Word::GetIndexOfMeaning(const std::wstring& meaning) const noexcept {
	const auto iter = std::find_if(m_Meanings.begin(), m_Meanings.end(), [&](const auto& meaningPtr) {
		return meaningPtr->GetMeaning() == meaning;
	});
	if (iter != m_Meanings.end()) return std::distance(m_Meanings.begin(), iter);
	else return -1;
}
const Meaning* Word::FindMeaning(const std::wstring& meaning) const noexcept {
	if (const auto index = GetIndexOfMeaning(meaning); index != -1) return m_Meanings[index].get();
	else return nullptr;
}
Meaning* Word::FindMeaning(const std::wstring& meaning) noexcept {
	if (const auto index = GetIndexOfMeaning(meaning); index != -1) return m_Meanings[index].get();
	else return nullptr;
}

Meaning Word::MergeMeaning(const std::wstring& separator) {
	if (m_Meanings.size() == 1) return *m_Meanings.front();

	std::wostringstream meaningStream, pronunciationStream;
	bool isFirstMeaning = true;
	std::unordered_set<std::wstring_view> usedPronunciations;
	for (const auto& meaningPtr : m_Meanings) {
		meaningStream << (isFirstMeaning ? L"" : separator) << meaningPtr->GetMeaning();
		isFirstMeaning = false;

		if (const auto pronunciation = meaningPtr->GetPronunciation();
			!pronunciation.empty() && !usedPronunciations.contains(pronunciation)) {
			pronunciationStream << (usedPronunciations.empty() ? L"" : separator) << pronunciation;
			usedPronunciations.insert(pronunciation);
		}
	}

	return { this, meaningStream.str(), pronunciationStream.str() };
}

void Word::CopyFrom(const Word& word) {
	m_Word = word.m_Word;

	for (const auto& meaningPtr : word.m_Meanings) {
		m_Meanings.push_back(std::make_unique<Meaning>(*meaningPtr, this));
	}
}
void Word::MoveFrom(Word&& word) noexcept {
	m_Word = std::move(word.m_Word);

	m_Meanings = std::move(word.m_Meanings);
	for (const auto& meaningPtr : m_Meanings) {
		meaningPtr->SetWord(this);
	}
}

Vocabulary::Vocabulary(const Vocabulary& vocabulary) {
	CopyFrom(vocabulary);
}

Vocabulary& Vocabulary::operator=(const Vocabulary& vocabulary) {
	CopyFrom(vocabulary);
	return *this;
}
const Word* Vocabulary::operator[](std::size_t index) const noexcept {
	return m_Words[index].get();
}
Word* Vocabulary::operator[](std::size_t index) noexcept {
	return m_Words[index].get();
}

std::size_t Vocabulary::GetCountOfWords() const noexcept {
	return m_Words.size();
}
void Vocabulary::AddWord(std::unique_ptr<Word>&& word) {
	m_Words.push_back(std::move(word));
}
void Vocabulary::RemoveWord(std::size_t index) {
	m_Words.erase(m_Words.begin() + index);
}
std::size_t Vocabulary::GetIndexOfWord(const std::wstring& word) const noexcept {
	const auto iter = std::find_if(m_Words.begin(), m_Words.end(), [&](const auto& wordPtr) {
		return wordPtr->GetWord() == word;
	});
	if (iter != m_Words.end()) return std::distance(m_Words.begin(), iter);
	else return -1;
}
const Word* Vocabulary::FindWord(const std::wstring& word) const noexcept {
	if (const auto index = GetIndexOfWord(word); index != -1) return m_Words[index].get();
	else return nullptr;
}
Word* Vocabulary::FindWord(const std::wstring& word) noexcept {
	if (const auto index = GetIndexOfWord(word); index != -1) return m_Words[index].get();
	else return nullptr;
}
Meaning* Vocabulary::ExtractRandomMeaning() noexcept {
	Word& word = *m_Words[GetRandomInteger(0, static_cast<int>(m_Words.size()))];
	return word[GetRandomInteger(0, static_cast<int>(word.GetCountOfMeanings()))];
}

bool Vocabulary::Load(const std::wstring& path) {
	std::ifstream file(path, std::ofstream::binary);
	if (!file) return false;

	m_Words.clear();

	LoadFromStream(file);
	return true;
}
bool Vocabulary::Save(const std::wstring& path) const {
	std::ofstream file(path, std::ofstream::binary);
	if (!file) return false;

	SaveToStream(file);
	return true;
}
bool Vocabulary::Export(const std::wstring& path, ExportType exportType) const {
	std::ofstream file(path, std::ofstream::binary);
	if (!file) return false;

	switch (exportType) {
	case Csv: ExportAsCsv(file, true); break;
	case CsvS: ExportAsCsv(file, false); break;
	default: return false;
	}
	return true;
}

void Vocabulary::CopyFrom(const Vocabulary& vocabulary) {
	for (const auto& wordPtr : vocabulary.m_Words) {
		m_Words.push_back(std::make_unique<Word>(*wordPtr));
	}
}

namespace {
	template<typename T>
	T Read(std::ifstream& file) {
		T result;
		file.read(reinterpret_cast<char*>(&result), sizeof(result));
		return result;
	}
	template<>
	std::wstring Read<std::wstring>(std::ifstream& file) {
		const int length = Read<int>(file);

		std::wstring result(length, 0);
		file.read(reinterpret_cast<char*>(result.data()), sizeof(wchar_t) * length);
		return result;
	}
	template<typename T>
	void Write(std::ofstream& file, T data) {
		file.write(reinterpret_cast<char*>(&data), sizeof(data));
	}
	void Write(std::ofstream& file, const std::wstring& data) {
		Write(file, static_cast<int>(data.size()));
		file.write(reinterpret_cast<const char*>(data.data()), sizeof(wchar_t) * data.size());
	}
	void Write(std::ofstream& file, const std::wstring_view& data) {
		Write(file, static_cast<int>(data.size()));
		file.write(reinterpret_cast<const char*>(data.data()), sizeof(wchar_t) * data.size());
	}
}

void Vocabulary::LoadFromStream(std::ifstream& file) {
	const int countOfWords = Read<int>(file);
	for (int i = 0; i < countOfWords; ++i) {
		std::wstring word = Read<std::wstring>(file);
		std::wstring pronunciation = Read<std::wstring>(file);
		std::wstring meaning = Read<std::wstring>(file);
		m_Words.push_back(std::make_unique<Word>(std::move(word), std::move(meaning), std::move(pronunciation)));
	}

	const int countOfContainers = Read<int>(file);
	if (file.eof()) return;

	for (int i = 0; i < countOfContainers; ++i) {
		const ContainerId containerId = Read<ContainerId>(file);
		const int containerLength = Read<int>(file);
		switch (containerId) {
		case HomonymContainer:
			ReadHomonymContainer(file);
			break;

		default:
			file.seekg(containerLength, std::ifstream::cur);
			break;
		}
	}
}
void Vocabulary::SaveToStream(std::ofstream& file) const {
	bool needHomonymContainer = false;

	Write(file, static_cast<int>(m_Words.size()));
	for (const auto& wordPtr : m_Words) {
		const Meaning mergedMeaning = wordPtr->MergeMeaning(L", ");
		needHomonymContainer = wordPtr->GetCountOfMeanings() > 1;

		Write(file, wordPtr->GetWord());
		Write(file, mergedMeaning.GetPronunciation());
		Write(file, mergedMeaning.GetMeaning());
	}

	if (needHomonymContainer) {
		Write(file, 1);
		WriteHomonymContainer(file);
	}
}

std::streampos Vocabulary::WriteContainerHeader(std::ofstream& file, ContainerId id) const {
	Write(file, id);

	const std::streampos lengthPos = file.tellp();
	Write(file, 0xFFFFFFFF);
	return lengthPos;
}
void Vocabulary::WriteContainerLength(std::ofstream& file, std::streampos lengthPos) const {
	const std::streampos endPos = file.tellp();

	file.seekp(lengthPos);
	Write(file, static_cast<int>(endPos - lengthPos - sizeof(int)));
	file.seekp(0, std::ofstream::end);
}
void Vocabulary::ReadHomonymContainer(std::ifstream& file) {
	for (auto& wordPtr : m_Words) {
		wordPtr->RemoveMeaning(0);

		const int countOfMeanings = Read<int>(file);
		for (int i = 0; i < countOfMeanings; ++i) {
			std::wstring pronunciation = Read<std::wstring>(file);
			std::wstring meaning = Read<std::wstring>(file);
			wordPtr->AddMeaning(std::make_unique<Meaning>(wordPtr.get(), std::move(meaning), std::move(pronunciation)));
		}
	}
}
void Vocabulary::WriteHomonymContainer(std::ofstream& file) const {
	const std::streampos lengthPos = WriteContainerHeader(file, HomonymContainer);

	for (const auto& wordPtr : m_Words) {
		const int countOfMeanings = static_cast<int>(wordPtr->GetCountOfMeanings());
		Write(file, countOfMeanings);

		for (int i = 0; i < countOfMeanings; ++i) {
			const Meaning* const meaning = (*wordPtr)[i];
			Write(file, meaning->GetPronunciation());
			Write(file, meaning->GetMeaning());
		}
	}

	WriteContainerLength(file, lengthPos);
}

void Vocabulary::ExportAsCsv(std::ofstream& file, bool insertBOM) const {
	if (insertBOM) {
		Write<unsigned char>(file, 0xEF);
		Write<unsigned char>(file, 0xBB);
		Write<unsigned char>(file, 0xBF);
	}

	for (const auto& wordPtr : m_Words) {
		const Meaning mergedMeaning = wordPtr->MergeMeaning(L",\n");
		const std::u8string wordUTF8 = EncodeToUTF8(wordPtr->GetWord());
		const std::u8string meaningUTF8 = EncodeToUTF8(mergedMeaning.GetMeaning());
		const std::u8string pronunciationUTF8 = EncodeToUTF8(mergedMeaning.GetPronunciation());

		const auto writeWithQuotes = [&](const std::u8string& string) {
			file << '"';
			file.write(reinterpret_cast<const char*>(string.data()), string.size());
			file << '"';
		};

		writeWithQuotes(wordUTF8);
		file << ',';
		writeWithQuotes(meaningUTF8);
		file << ',';
		writeWithQuotes(pronunciationUTF8);
		file << '\n';
	}
}