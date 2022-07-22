#include <algorithm>
#include <iostream>
#include <set>
#include <string>
#include <utility>
#include <vector>
#include <map>

using namespace std::literals;

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

std::set<std::string> VectorToSet(const std::vector<std::string>& vs) {
    return { vs.begin(), vs.end() };
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

void AddDocument(
    std::vector<std::pair<int, std::vector<std::string>>>& documents, 
    const std::set<std::string>& stop_words,
    int document_id,
    const std::string& document) {
    
    const std::vector<std::string> words = SplitIntoWordsNoStop(document, stop_words);
    documents.push_back({ document_id, words });
}

std::set<std::string> ParseQuery(const std::string& text, const std::set<std::string>& stop_words) {
    std::set<std::string> query_words = VectorToSet(SplitIntoWordsNoStop(text, stop_words));
    return query_words;
}

int MatchDocument(const std::pair<int, std::vector<std::string>>& content, 
    const std::set<std::string>& query_words) {

    int relevance = 0;

    const auto& [id, doc] = content;

    for (const std::string& word : VectorToSet(doc)) {
        std::set<std::string>::const_iterator w = query_words.find(word);
        if (w != query_words.end()) relevance++;
    }

    return relevance;
}

std::vector<std::pair<int, int>> FindDocuments(
    const std::vector<std::pair<int, std::vector<std::string>>>& documents, 
    const std::set<std::string>& stop_words,
    const std::string& query) {

    const std::set<std::string> query_words = ParseQuery(query, stop_words);
    std::vector<std::pair<int, int>> matched_documents;

    for (const auto& [id, document] : documents) {
        int relevance = MatchDocument({ id, document }, query_words);
        if (relevance > 0) {
            matched_documents.push_back({ id, relevance });
        }
    }    

    return matched_documents;
}

int main() {
    const std::string stop_words_joined = ReadLine();
    const std::set<std::string> stop_words = ParseStopWords(stop_words_joined);

    // Read documents
    std::vector<std::pair<int, std::vector<std::string>>> documents;
    const int document_count = ReadLineWithNumber();
    for (int document_id = 0; document_id < document_count; ++document_id) {
        AddDocument(documents, stop_words, document_id, ReadLine());
    }

    const std::string query = ReadLine();
    for (const auto& [id, relevance] : FindDocuments(documents, stop_words, query)) {
        std::cout << "{ document_id = "s << id << ", relevance = "s<<relevance<<" }"s << std::endl;
    }
}