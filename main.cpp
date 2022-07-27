#include <iostream>
#include <set>
#include <string>
#include <utility>
#include <vector>
#include <algorithm>

const int MAX_RESULT_DOCUMENT_COUNT = 5;

using namespace std;


struct Document {
    int id, relevance;
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

std::set<std::string> ParseMinusWords(const std::set<std::string>& query) {
    std::set<std::string> minus_words;
    for (const std::string& word : query) {
        if (word[0] == '-') {
            minus_words.insert(word.substr(1));
        }
    }
    return minus_words;
}

class SearchServer {
    struct DocumentContent {
        int id;
        std::vector<std::string> words;
    };

    std::vector<DocumentContent> documents_;
    std::set<std::string> stop_words_;

private:

    std::set<std::string> ParseQuery(const std::string& text) const {
        std::set<std::string> query_words;
        for (const std::string& word : SplitIntoWordsNoStop(text)) {
            query_words.insert(word);
        }
        return query_words;
    }

    std::vector<std::string> SplitIntoWordsNoStop(const std::string& text) const {
        std::vector<std::string> words;
        for (const std::string& word : SplitIntoWords(text)) {
            if (stop_words_.count(word) == 0) {
                words.push_back(word);
            }
        }
        return words;
    }    

    std::vector<Document> FindAllDocuments(const std::set<std::string>& query_words) const{
        std::vector<Document> matched_documents;
        for (const auto& document : documents_) {
            const int relevance = MatchDocument(document, query_words);
            if (relevance > 0) {
                matched_documents.push_back({ document.id, relevance });
            }
        }
        return matched_documents;
    }

public:
    void AddDocument(
        int document_id,
        const std::string& document) {

        const std::vector<std::string> words = SplitIntoWordsNoStop(document);
        documents_.push_back({ document_id, words });
    }

    void SetStopWords(const std::string& text) {
        for (const std::string& word : SplitIntoWords(text)) {
            stop_words_.insert(word);
        }
    }

    std::vector<Document> FindTopDocuments(const std::string& raw_query) const {
        const std::set<std::string> query_words = ParseQuery(raw_query);

        auto matched_documents = FindAllDocuments(query_words);
        std::sort(matched_documents.begin(), matched_documents.end(), [](const Document& lhs, const Document& rhs) {
            return lhs.relevance > rhs.relevance;
        });

        if (matched_documents.size() > MAX_RESULT_DOCUMENT_COUNT) {
            matched_documents.resize(MAX_RESULT_DOCUMENT_COUNT);
        }

        return matched_documents;
    }

    static int MatchDocument(const DocumentContent& content, const std::set<std::string>& query_words) {        

        if (query_words.empty()) {
            return 0;
        }

        std::set<std::string> minus_words = ParseMinusWords(query_words);
        for (const std::string& m_word : minus_words) {
            if (std::count(content.words.begin(), content.words.end(), m_word) > 0) return 0;
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
};

SearchServer CreateSearchServer() {
    SearchServer server;
    const std::string stop_words_joined = ReadLine();
    server.SetStopWords(stop_words_joined);

    const int document_count = ReadLineWithNumber();    
    
    for (int document_id = 0; document_id < document_count; ++document_id) {
        server.AddDocument(document_id, ReadLine());
    }

    return server;
}

int main() {
    
    const SearchServer server = CreateSearchServer();

    const std::string query = ReadLine();

    for (auto [document_id, relevance] : server.FindTopDocuments(query)) {
        std::cout << "{ document_id = "s << document_id << ", relevance = "s << relevance << " }"s
            << std::endl;
    }
}