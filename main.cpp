#include <iostream>
#include <set>
#include <string>
#include <utility>
#include <vector>
#include <algorithm>
#include <map>
#include <cmath>

const int MAX_RESULT_DOCUMENT_COUNT = 5;

using namespace std::literals;

struct Document {
    int id;
    double relevance;
    int rating;
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

std::vector<int> FillRating() {
    std::vector<int> ratings;
    int rc;
    std::cin >> rc;
    for (int i = 0; i < rc; ++i) {
        int r;
        std::cin >> r;
        ratings.push_back(r);
    }
    ReadLine();
    return ratings;
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

class SearchServer {

    struct Query {
        std::set<std::string> plus_words;
        std::set<std::string> minus_words;
    };

    struct QueryWord {
        std::string data;
        bool is_minus;
        bool is_stop;
    };

    std::map<std::string, std::map<int, double>> word_to_document_freqs_;
    std::set<std::string> stop_words_;
    std::set<int> document_idx_;
    std::map<int, int> rating_of_documents_;

private:

    //-------------------------------IsStopWord-------------------------------
    bool IsStopWord(const std::string& word)const {
        return stop_words_.count(word) > 0;
    }

    //-------------------------------ParseQueryWord-------------------------------
    QueryWord ParseQueryWord(std::string text)const {
        bool is_minus = false;
        if (text[0] == '-') {
            is_minus = true;
            text = text.substr(1);
        }
        return { text, is_minus, IsStopWord(text) };
    }

    //-------------------------------ParseQuery-------------------------------
    Query ParseQuery(const std::string& text) const {
        Query query;

        for (const std::string& word : SplitIntoWords(text)) {
            const QueryWord query_word = ParseQueryWord(word);
            if (!query_word.is_stop) {
                if (query_word.is_minus) {
                    query.minus_words.insert(query_word.data);
                }
                else {
                    query.plus_words.insert(query_word.data);
                }
            }
        }
        return query;
    }

    //-------------------------------SplitIntoWordsNoStop-------------------------------
    std::vector<std::string> SplitIntoWordsNoStop(const std::string& text) const {
        std::vector<std::string> words;
        for (const std::string& word : SplitIntoWords(text)) {
            if (stop_words_.count(word) == 0) {
                words.push_back(word);
            }
        }
        return words;
    }

    //-------------------------------FindAllDocuments-------------------------------
    std::vector<Document> FindAllDocuments(const Query& query) const {
        std::map<int, double> document_to_relevance;

        for (const std::string& plus_word : query.plus_words) {
            if (word_to_document_freqs_.count(plus_word) == 0) {
                continue;
            }
            for (const auto& [document_id, tf] : word_to_document_freqs_.at(plus_word)) {
                document_to_relevance[document_id] += tf * std::log(document_idx_.size() / static_cast<double>(word_to_document_freqs_.at(plus_word).size()));
            }
        }

        for (const std::string& minus_word : query.minus_words) {
            if (word_to_document_freqs_.count(minus_word) == 0) {
                continue;
            }            
            for (const auto [id, _] : word_to_document_freqs_.at(minus_word)) {
                    document_to_relevance.erase(id);
            }                
        }

        std::vector<Document> matched_documents;

        for (const auto [id, relevance] : document_to_relevance) {
            matched_documents.push_back({ id, relevance, rating_of_documents_.at(id)});
        }

        return matched_documents;
    }

    static int ComputeAverageRating(const std::vector<int>& ratings) {
        int sum = 0;
        for (int r : ratings) {
            sum += r;
        }
        return (ratings.size() > 0)?sum / static_cast<int>(ratings.size()):0;
    }

public:

    //-------------------------------AddDocument-------------------------------
    void AddDocument(
        int document_id,
        const std::string& document,
        const std::vector<int>& ratings) {

        const std::vector<std::string> words = SplitIntoWordsNoStop(document);
        for (const std::string& word : words) {
            word_to_document_freqs_[word][document_id] += 1.0 / static_cast<double>(words.size());
        }
        document_idx_.insert(document_id);
        rating_of_documents_[document_id] = ComputeAverageRating(ratings);
    }

    //-------------------------------SetStopWords-------------------------------
    void SetStopWords(const std::string& text) {
        for (const std::string& word : SplitIntoWords(text)) {
            stop_words_.insert(word);
        }
    }

    //-------------------------------FindTopDocuments-------------------------------
    std::vector<Document> FindTopDocuments(const std::string& raw_query) const {
        const Query query = ParseQuery(raw_query);

        auto matched_documents = FindAllDocuments(query);
        std::sort(matched_documents.begin(), matched_documents.end(), [](const Document& lhs, const Document& rhs) {
            return lhs.relevance > rhs.relevance;
            });

        if (matched_documents.size() > MAX_RESULT_DOCUMENT_COUNT) {
            matched_documents.resize(MAX_RESULT_DOCUMENT_COUNT);
        }

        return matched_documents;
    }
};

SearchServer CreateSearchServer() {
    SearchServer server;
    const std::string stop_words_joined = ReadLine();
    server.SetStopWords(stop_words_joined);

    const int document_count = ReadLineWithNumber();

    for (int document_id = 0; document_id < document_count; ++document_id) {
        
        const std::string query = ReadLine();
        const std::vector<int> rating = FillRating();
        
        server.AddDocument(document_id, query, rating);
    }

    return server;
}

int main() {

    const SearchServer server = CreateSearchServer();

    const std::string query = ReadLine();

    for (auto [document_id, relevance, rating] : server.FindTopDocuments(query)) {
        std::cout << "{ document_id = "s << document_id << ", relevance = "s << relevance << ", rating = "s<<rating<<" }"s
            << std::endl;
    }
}