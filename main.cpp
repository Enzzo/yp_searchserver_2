#include <iostream>
#include <set>
#include <string>
#include <utility>
#include <vector>
#include <algorithm>

const int MAX_RESULT_DOCUMENT_COUNT = 5;

using namespace std::literals;

struct DocumentContent {
    int id;
    std::vector<std::string> words;
};

std::string ReadLine() {
    std::string s;
    std::getline(std::cin, s);
    return s;
}

int ReadLineWithNumber() {
    int result = 0;
    std::cin >> result;
    ReadLine();
    return result;
}

std::vector<std::string> SplitIntoWords(const std::string& text) {
    std::vector<std::string> words;
    std::string word;
    for (const char c : text) {
        if (c == ' ') {
            if (!word.empty()) {
                words.push_back(word);
                word.clear();
            }
        }
        else {
            word += c;
        }
    }
    if (!word.empty()) {
        words.push_back(word);
    }

    return words;
}

std::set<std::string> ParseStopWords(const std::string& text) {
    std::set<std::string> stop_words;
    for (const std::string& word : SplitIntoWords(text)) {
        stop_words.insert(word);
    }
    return stop_words;
}

std::vector<std::string> SplitIntoWordsNoStop(const std::string& text, const std::set<std::string>& stop_words) {
    std::vector<std::string> words;
    for (const std::string& word : SplitIntoWords(text)) {
        if (stop_words.count(word) == 0) {
            words.push_back(word);
        }
    }
    return words;
}

void AddDocument(std::vector<DocumentContent>& documents, 
    const std::set<std::string>& stop_words,
    int document_id, const std::string& document) {

    const std::vector<std::string> words = SplitIntoWordsNoStop(document, stop_words);
    documents.push_back({document_id, words});
}

std::set<std::string> ParseQuery(const std::string& text, const std::set<std::string>& stop_words) {
    std::set<std::string> query_words;
    for (const std::string& word : SplitIntoWordsNoStop(text, stop_words)) {
        query_words.insert(word);
    }
    return query_words;
}

int MatchDocument(
    const DocumentContent& content, 
    const std::set<std::string>& query_words) {

    if (query_words.empty()) {
        return 0;
    }
    std::set<std::string> matched_words;
    for (const std::string& word : content.words) {
        if (matched_words.count(word) != 0) {
            continue;
        }
        if (query_words.count(word) != 0) {
            matched_words.insert(word);
        }
    }
    return static_cast<int>(matched_words.size());
}

std::vector<std::pair<int, int>> FindAllDocuments(
    const std::vector<DocumentContent>& documents,
    const std::set<std::string>& query_words)
{
    
    std::vector<std::pair<int, int>> matched_documents;
    for (const auto& document : documents) {
        const int relevance = MatchDocument(document, query_words);
        if (relevance > 0) {
            matched_documents.push_back({ relevance, document.id });
        }
    }
    return matched_documents;
}

std::vector<std::pair<int, int>> FindTopDocuments(
    const std::vector<DocumentContent>& documents,
    const std::set<std::string>& stop_words, 
    const std::string& raw_query) {
    const std::set<std::string> query_words = ParseQuery(raw_query, stop_words);

    auto matched_documents = FindAllDocuments(documents, query_words);
    std::sort(matched_documents.rbegin(), matched_documents.rend());

    if (matched_documents.size() > MAX_RESULT_DOCUMENT_COUNT) {
        matched_documents.resize(MAX_RESULT_DOCUMENT_COUNT);
    }

    for (auto& [id, relevance] : matched_documents) {
        std::swap(id, relevance);
    }
    return matched_documents;
}

int main() {
    const std::string stop_words_joined = ReadLine();
    const std::set<std::string> stop_words = ParseStopWords(stop_words_joined);

    // Read documents
    std::vector<DocumentContent> documents;
    const int document_count = ReadLineWithNumber();
    for (int document_id = 0; document_id < document_count; ++document_id) {
        AddDocument(documents, stop_words, document_id, ReadLine());
    }

    const std::string query = ReadLine();
    for (auto [document_id, relevance] : FindTopDocuments(documents, stop_words, query)) {
        std::cout << "{ document_id = "s << document_id << ", relevance = "s << relevance << " }"s
            << std::endl;
    }
}